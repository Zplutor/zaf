#include <zaf/internal/window/window_focused_control_manager.h>
#include <zaf/base/auto_reset.h>
#include <zaf/input/keyboard.h>
#include <zaf/internal/control/control_event_route_utility.h>
#include <zaf/internal/tab_stop_utility.h>

namespace zaf::internal {

WindowFocusedControlManager::WindowFocusedControlManager(Window& window) : window_(window) {

}


bool WindowFocusedControlManager::TryToPreprocessTabKeyMessage(const KeyMessage& message) {

    if (message.ID() != WM_KEYDOWN || message.Key() != Key::Tab) {
        return false;
    }

    if ((focused_control_) && focused_control_->AcceptKeyMessage(message)) {
        return false;
    }

    bool backward = Keyboard::IsShiftDown();
    SwitchFocusedControlByTabKey(backward);
    return true;
}


void WindowFocusedControlManager::SwitchFocusedControlByTabKey(bool backward) {

    auto current_focused_control = focused_control_;
    if (!current_focused_control) {
        current_focused_control = window_.RootControl();
    }

    auto next_focused_control = internal::FindNextTabStopControl(
        current_focused_control, 
        backward);

    if (next_focused_control) {
        ChangeFocusedControl(next_focused_control);
    }
}


void WindowFocusedControlManager::HandleWindowSetFocus() {

    auto last_focused_control = last_focused_control_.lock();
    if (!last_focused_control) {
        return;
    }

    last_focused_control_.reset();
    ChangeFocusedControl(last_focused_control);
}


void WindowFocusedControlManager::HandleWindowKillFocus() {

    last_focused_control_ = focused_control_;
    ChangeFocusedControl(nullptr);
}


void WindowFocusedControlManager::HandleWindowDestroy() {

    if (focused_control_) {
        focused_control_->SetIsFocusedByWindow(false);
        focused_control_ = nullptr;
    }
}


void WindowFocusedControlManager::ChangeFocusedControl(
    const std::shared_ptr<Control>& new_focused_control) {

    if (!CheckIfCanSetFocusedControl(new_focused_control)) {
        return;
    }

    //This method is re-entrant, we use a counter to check if there is reentrance.
    std::optional<AutoReset<std::size_t>> counter_auto_reset;
    if (change_focus_counter_ == 0) {
        counter_auto_reset = MakeAutoReset(change_focus_counter_);
    }

    auto current_counter = ++change_focus_counter_;

    auto previous_focused_control = focused_control_;
    focused_control_ = new_focused_control;

    //We must change the focus states of the controls immediately to meet the constraint.
    if (previous_focused_control) {
        previous_focused_control->SetIsFocusedByWindow(false);
    }
    
    if (focused_control_) {
        focused_control_->SetIsFocusedByWindow(true);
    }

    //We must use new_focused_control rather than focused_control_ to do event routing, as 
    //focused_control_ might be changed during event routing.
    if (previous_focused_control) {
        RouteFocusEvent(previous_focused_control, new_focused_control, false);
    }

    //If a reentrance occurs, new_focused_control is no longer the focused control, don't 
    //change its focus state.
    if (current_counter == change_focus_counter_) {
        if (new_focused_control) {
            RouteFocusEvent(new_focused_control, previous_focused_control, true);
        }
    }

    //Only the first setting raises focused control changed event.
    if (counter_auto_reset) {
        counter_auto_reset.reset();

        //Raise the event only if the focused control is actually changed.
        if (focused_control_ != previous_focused_control) {
            focused_control_changed_event_.Raise(previous_focused_control);
        }
    }
}


bool WindowFocusedControlManager::CheckIfCanSetFocusedControl(
    const std::shared_ptr<Control>& new_focused_control) const {

    //The same control, not allow to change.
    if (focused_control_ == new_focused_control) {
        return false;
    }

    if (new_focused_control) {

        //Not allow to set focused control if the window has no focus.
        //(But allow to clear focused control)
        if (!window_.IsFocused()) {
            return false;
        }

        if (!new_focused_control->IsEnabledInContext()) {
            return false;
        }

        //The focused control must be contained in this window
        if (new_focused_control->Window().get() != &window_) {
            return false;
        }
    }

    return true;
}


void WindowFocusedControlManager::RouteFocusEvent(
    const std::shared_ptr<Control>& target_control,
    const std::shared_ptr<Control>& changing_control,
    bool is_focused) {

    auto event_info_state = std::make_shared<FocusEventSharedState>(
        target_control,
        changing_control);

    if (TunnelFocusEvent(target_control, event_info_state, is_focused)) {
        BubbleFocusEvent(target_control, event_info_state, is_focused);
    }
}


bool WindowFocusedControlManager::TunnelFocusEvent(
    const std::shared_ptr<Control>& target_control,
    const std::shared_ptr<FocusEventSharedState>& event_state, 
    bool is_focused) {

    auto tunnel_path = BuildTunnelPath(target_control);
    if (tunnel_path.empty()) {
        return false;
    }

    auto tunnel_event_invoker = 
        is_focused ?
        [](const std::shared_ptr<FocusEventSharedState>& state,
           const std::shared_ptr<Control>& sender) {
            sender->OnPreFocusGained(PreFocusGainedInfo{ state, sender });
        }
        :
        [](const std::shared_ptr<FocusEventSharedState>& state,
           const std::shared_ptr<Control>& sender) {
            sender->OnPreFocusLost(PreFocusLostInfo{ state, sender });
        };

    bool original_is_focused = target_control->IsFocused();

    for (const auto& each_control : tunnel_path) {

        tunnel_event_invoker(event_state, each_control);

        //Stop event routing if focus is changed during routing.
        if (original_is_focused != target_control->IsFocused()) {
            return false;
        }
    }

    return true;
}


void WindowFocusedControlManager::BubbleFocusEvent(
    const std::shared_ptr<Control>& target_control, 
    const std::shared_ptr<FocusEventSharedState>& event_state, 
    bool is_focused) {

    auto bubble_event_invoker = 
        is_focused ?
        [](const std::shared_ptr<FocusEventSharedState>& state,
           const std::shared_ptr<Control>& sender) {
            sender->OnFocusGained(FocusGainedInfo{ state, sender });
        }
        :
        [](const std::shared_ptr<FocusEventSharedState>& state,
           const std::shared_ptr<Control>& sender) {
            sender->OnFocusLost(FocusLostInfo{ state, sender });
        };

    bool original_is_focused = target_control->IsFocused();

    auto sender = target_control;
    while (sender) {

        bubble_event_invoker(event_state, sender);

        //Stop event routing if focus is changed during routing.
        if (original_is_focused != target_control->IsFocused()) {
            break;
        }

        sender = sender->Parent();
    }
}

}