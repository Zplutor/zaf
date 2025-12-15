#include <zaf/window/internal/window_facets/window_focus_facet.h>
#include <zaf/internal/window/window_focused_control_manager.h>
#include <zaf/window/window.h>

namespace zaf::internal {

WindowFocusFacet::WindowFocusFacet(Window& window) noexcept : 
    window_(window),
    focused_control_manager_(window) {

    window_.Disposables() += focused_control_manager_.FocusedControlChangedEvent().Subscribe(
        [this](const std::shared_ptr<Control>& previous_focused_control) {

            window_.OnFocusedControlChanged(FocusedControlChangedInfo{
                window_.shared_from_this(),
                previous_focused_control
            });
        });
}


bool WindowFocusFacet::BringToForeground() noexcept {

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::Creating ||
        handle_state == WindowHandleState::Created ||
        handle_state == WindowHandleState::Destroying) {
        return !!SetForegroundWindow(window_.Handle());
    }
    return false;
}


void WindowFocusFacet::HandleWMACTIVATE(const ActivateMessage& message) {

    internal::ActivateEventInfo event_info{ window_.shared_from_this(), message };

    if (event_info.Message().ActivateState() != ActivateState::Inactive) {
        window_.OnActivated(event_info);
    }
    else {
        window_.OnDeactivated(event_info);
    }
}


LRESULT WindowFocusFacet::HandleWMMOUSEACTIVATE() {

    auto activate_option = window_.ActivateOptions();
    bool no_activate = HasFlag(activate_option, ActivateOptions::NoMouseActivate);
    bool discard_message = HasFlag(activate_option, ActivateOptions::DiscardMouseMessage);
    if (no_activate) {
        return discard_message ? MA_NOACTIVATEANDEAT : MA_NOACTIVATE;
    }
    else {
        return discard_message ? MA_ACTIVATEANDEAT : MA_ACTIVATE;
    }
}


bool WindowFocusFacet::IsFocused() const noexcept {
    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::Creating ||
        handle_state == WindowHandleState::Created ||
        handle_state == WindowHandleState::Destroying) {
        return GetFocus() == window_.Handle();
    }
    return false;
}


void WindowFocusFacet::HandleWMSETFOCUS(const Message& message) {
    focused_control_manager_.HandleWindowSetFocus();
    window_.OnFocusGained(WindowFocusGainedInfo{ window_.shared_from_this(), message });
}


void WindowFocusFacet::HandleWMKILLFOCUS(const Message& message) {
    focused_control_manager_.HandleWindowKillFocus();
    window_.OnFocusLost(WindowFocusLostInfo{ window_.shared_from_this(), message });
}


bool WindowFocusFacet::TryToPreprocessTabKeyMessage(const KeyMessage& message) {
    return focused_control_manager_.TryToPreprocessTabKeyMessage(message);
}


void WindowFocusFacet::ChangeFocusedControl(const std::shared_ptr<Control>& new_focused_control) {
    focused_control_manager_.ChangeFocusedControl(new_focused_control);
}


std::shared_ptr<Control> WindowFocusFacet::FocusedControl() const noexcept {
    return focused_control_manager_.FocusedControl();
}


void WindowFocusFacet::HandleWindowDestroy() {
    focused_control_manager_.HandleWindowDestroy();
}

}