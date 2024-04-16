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

    //This method is re-entrant. If there is already a control that is being set focus, we store 
    //the last contorl to a pending variable, and set the focus to the pending control after 
    //current setting is finished.
    if (is_changing_focused_control_) {
        pending_focused_control_ = new_focused_control;
        return;
    }

    {
        auto auto_reset = MakeAutoReset(is_changing_focused_control_, true);
        SetFocusedControl(new_focused_control);
    }

    //Recursively set focus to the pending control.
    if (pending_focused_control_) {

        auto pending_focused_control = pending_focused_control_->lock();
        pending_focused_control_.reset();

        ChangeFocusedControl(pending_focused_control);
    }
}


void WindowFocusedControlManager::SetFocusedControl(
    const std::shared_ptr<Control>& new_focused_control) {

    if (!CheckIfCanSetFocusedControl(new_focused_control)) {
        return;
    }

    auto previous_focused_control = focused_control_;
    focused_control_ = new_focused_control;

    if (previous_focused_control) {
        ChangeControlFocusState(previous_focused_control, new_focused_control, false);
    }

    if (new_focused_control) {
        ChangeControlFocusState(new_focused_control, previous_focused_control, true);
    }

    focused_control_changed_event_.Raise(previous_focused_control);
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


void WindowFocusedControlManager::ChangeControlFocusState(
    const std::shared_ptr<Control>& target_control,
    const std::shared_ptr<Control>& changing_control,
    bool is_focused) {

    target_control->SetIsFocusedByWindow(is_focused);

    RouteFocusEvent(target_control, changing_control, is_focused);
}


void WindowFocusedControlManager::RouteFocusEvent(
    const std::shared_ptr<Control>& target_control,
    const std::shared_ptr<Control>& changing_control,
    bool is_focused) {

    auto event_info_state = std::make_shared<FocusEventSharedState>(
        target_control,
        changing_control);

    TunnelFocusEvent(target_control, event_info_state, is_focused);
    BubbleFocusEvent(target_control, event_info_state, is_focused);
}


void WindowFocusedControlManager::TunnelFocusEvent(
    const std::shared_ptr<Control>& target_control,
    const std::shared_ptr<FocusEventSharedState>& event_state, 
    bool is_focused) {

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
    }
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

    auto sender = target_control;
    while (sender) {

        bubble_event_invoker(event_state, sender);
        sender = sender->Parent();
    }
}

}