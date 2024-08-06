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

    auto focused_control = focused_control_.lock();
    if (focused_control && focused_control->AcceptKeyMessage(message)) {
        return false;
    }

    bool backward = Keyboard::IsShiftDown();
    SwitchFocusedControlByTabKey(backward);
    return true;
}


void WindowFocusedControlManager::SwitchFocusedControlByTabKey(bool backward) {

    auto current_focused_control = focused_control_.lock();
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

    auto focused_control = focused_control_.lock();
    if (focused_control) {
        focused_control->SetIsFocusedByWindow(false);
        focused_control_.reset();
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

    auto previous_focused_control = focused_control_.lock();
    focused_control_ = new_focused_control;

    //We must change the focus states of the controls immediately to meet the constraint.
    if (previous_focused_control) {
        previous_focused_control->SetIsFocusedByWindow(false);
    }
    
    if (new_focused_control) {
        new_focused_control->SetIsFocusedByWindow(true);
    }

    //We must use new_focused_control rather than focused_control_ to do event routing, as 
    //focused_control_ might be changed during event routing.
    if (previous_focused_control) {
        RouteFocusEvent(previous_focused_control, new_focused_control, false, current_counter);
    }

    //If a reentrance occurs, new_focused_control is no longer the focused control, don't 
    //change its focus state.
    if (current_counter == change_focus_counter_) {
        if (new_focused_control) {
            RouteFocusEvent(new_focused_control, previous_focused_control, true, current_counter);
        }
    }

    //Only the first setting raises focused control changed event.
    if (counter_auto_reset) {
        counter_auto_reset.reset();

        //Raise the event only if the focused control is actually changed.
        if (focused_control_.lock() != previous_focused_control) {
            focused_control_changed_event_.Raise(previous_focused_control);
        }
    }
}


bool WindowFocusedControlManager::CheckIfCanSetFocusedControl(
    const std::shared_ptr<Control>& new_focused_control) const {

    //The same control, not allow to change.
    if (focused_control_.lock() == new_focused_control) {
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
    bool is_focused,
    std::size_t current_counter) {

    auto event_info_state = std::make_shared<FocusEventSharedState>(
        target_control,
        changing_control);

    TunnelFocusEvent(target_control, event_info_state, is_focused, current_counter);

    //Stop event routing if there is reentrance.
    if (current_counter != change_focus_counter_) {
        return;
    }

    BubbleFocusEvent(target_control, event_info_state, is_focused, current_counter);
}


void WindowFocusedControlManager::TunnelFocusEvent(
    const std::shared_ptr<Control>& target_control,
    const std::shared_ptr<FocusEventSharedState>& event_state, 
    bool is_focused,
    std::size_t current_counter) {

    auto tunnel_path = BuildTunnelPath(target_control);
    if (tunnel_path.empty()) {
        return;
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

    for (const auto& each_control : tunnel_path) {

        tunnel_event_invoker(event_state, each_control);

        //Stop event routing if there is reentrance.
        if (current_counter != change_focus_counter_) {
            break;
        }
    }
}


void WindowFocusedControlManager::BubbleFocusEvent(
    const std::shared_ptr<Control>& target_control, 
    const std::shared_ptr<FocusEventSharedState>& event_state, 
    bool is_focused,
    std::size_t current_counter) {

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

    auto sender = target_control;
    while (sender) {

        //Controls at the bubble path should update their styles to reflect the change of focus 
        //state. For example, parents may use ContainsFocus property for their styles.
        sender->NeedUpdateStyle();
        bubble_event_invoker(event_state, sender);

        //Stop event routing if there is reentrance.
        if (current_counter != change_focus_counter_) {
            break;
        }

        sender = sender->Parent();
    }
}

}