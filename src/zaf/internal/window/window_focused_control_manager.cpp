#include <zaf/internal/window/window_focused_control_manager.h>
#include <zaf/base/auto_reset.h>
#include <zaf/input/keyboard.h>
#include <zaf/internal/tab_stop_utility.h>

namespace zaf::internal {
namespace {

template<typename E>
void RouteFocusEventGeneric(
    const std::shared_ptr<Control>& source,
    const std::shared_ptr<Control>& changing_control,
    void (Control::* event_function)(const E&)) {

    auto event_info_state = std::make_shared<internal::FocusEventSharedState>(
        source,
        changing_control);

    bool original_is_focused = source->IsFocused();

    for (auto sender = source; sender; sender = sender->Parent()) {

        E event_info{ event_info_state, sender };
        (sender.get()->*event_function)(event_info);

        //Stop routing event if focused control is changed during the routing.
        if (source->IsFocused() != original_is_focused) {
            break;
        }
    }
}

}

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

    if (focused_control_ == new_focused_control) {
        if (new_focused_control) {
            ChangeControlFocusState(new_focused_control, previous_focused_control, true);
        }
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

    if (is_focused) {

        RouteFocusEventGeneric<FocusGainedInfo>(
            target_control,
            changing_control,
            &Control::OnFocusGained);
    }
    else {

        RouteFocusEventGeneric<FocusLostInfo>(
            target_control,
            changing_control,
            &Control::OnFocusLost);
    }
}

}