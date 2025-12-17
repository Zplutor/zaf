#include <zaf/window/internal/window_facets/window_mouse_facet.h>
#include <zaf/internal/control/mouse_event_routing.h>
#include <zaf/window/internal/window_facets/window_lifecycle_facet.h>
#include <zaf/window/tooltip_window.h>
#include <zaf/window/window.h>

namespace zaf::internal {
namespace {

Point TranslateAbsolutePositionToControlPosition(
    const Point& absolute_position,
    const Control& control) {

    auto control_absolute_rect = control.RectInWindow();
    if (!control_absolute_rect) {
        return absolute_position;
    }

    Point result;
    result.x = absolute_position.x - control_absolute_rect->position.x;
    result.y = absolute_position.y - control_absolute_rect->position.y;
    return result;
}

} // namespace

WindowMouseFacet::WindowMouseFacet(Window& window) noexcept : window_(window) {

}


std::optional<HitTestResult> WindowMouseFacet::HitTest(const HitTestMessage& message) {

    auto mouse_over_control = [&]() {

        Point mouse_position = message.MousePosition();
        std::shared_ptr<Control> current_control = window_.root_control_;
        while (true) {

            auto child = current_control->FindChildAtPosition(mouse_position);
            if (child == nullptr) {
                break;
            }

            auto child_position = child->Position();
            mouse_position.x += child_position.x;
            mouse_position.y += child_position.y;

            current_control = child;
        }
        return current_control;
    }();

    return mouse_over_control->HitTest(message);
}


bool WindowMouseFacet::HandleGeneralMouseMessage(const MouseMessage& message) {

    if (message.ID() == WM_MOUSEMOVE || message.ID() == WM_NCMOUSEMOVE) {

        if (window_.is_selecting_inspector_control_) {
            window_.HighlightControlAtPosition(message.MousePosition());
            return true;
        }

        TrackMouseByMouseMove(message);
    }
    else {

        HideTooltipWindow();

        if (message.ID() == WM_LBUTTONDOWN || message.ID() == WM_RBUTTONDOWN) {

            if (window_.is_selecting_inspector_control_) {
                window_.SelectInspectedControl();
                return true;
            }
        }
    }

    return RouteMouseEvent(message);
}


void WindowMouseFacet::TrackMouseByMouseMove(const MouseMessage& message) {

    bool is_non_client = message.ID() == WM_NCMOUSEMOVE;

    auto is_tracking_mouse = [this, is_non_client]() {

        auto track_mode = window_.lifecycle_facet_->HandleStateData().track_mouse_mode;
        if (track_mode == TrackMouseMode::ClientArea && !is_non_client) {
            return true;
        }

        if (track_mode == TrackMouseMode::NonClientArea && is_non_client) {
            return true;
        }

        return false;
    }();

    if (is_tracking_mouse) {
        return;
    }

    TrackMouse(is_non_client);
}


void WindowMouseFacet::TrackMouse(bool is_non_client) {

    //Don't track mouse if the mouse is being captured and the capturing window is not the current 
    //window. This would happen if WM_MOUSEMOVE message is redirected from a capturing window to 
    //another window (this is the way how PopupMenu works).
    auto capturing_window = GetCapture();
    if (capturing_window && capturing_window != window_.Handle()) {
        return;
    }

    TRACKMOUSEEVENT track_mouse_event_param{};
    track_mouse_event_param.cbSize = sizeof(track_mouse_event_param);
    track_mouse_event_param.dwFlags = TME_LEAVE | TME_HOVER;
    if (is_non_client) {
        track_mouse_event_param.dwFlags |= TME_NONCLIENT;
    }
    track_mouse_event_param.hwndTrack = window_.Handle();

    if (TrackMouseEvent(&track_mouse_event_param)) {

        window_.lifecycle_facet_->HandleStateData().track_mouse_mode =
            is_non_client ?
            TrackMouseMode::NonClientArea :
            TrackMouseMode::ClientArea;
    }
}


void WindowMouseFacet::HandleMouseHoverMessage(const Message& message) {

    auto mouse_over_control = this->MouseOverControl();
    if (mouse_over_control) {

        //Raise and route mouse hover event.
        auto event_info_state = std::make_shared<RoutedEventSharedState>(mouse_over_control);
        for (auto control = mouse_over_control; control; control = control->Parent()) {
            control->OnMouseHover(MouseHoverInfo{ event_info_state, control });
        }
    }

    TryToShowTooltipWindow();
}


void WindowMouseFacet::HandleMouseLeaveMessage(const Message& message) {

    bool is_tracking_mouse = [&]() {

        auto track_mode = window_.lifecycle_facet_->HandleStateData().track_mouse_mode;
        if (track_mode == TrackMouseMode::ClientArea &&
            message.ID() == WM_MOUSELEAVE) {
            return true;
        }

        if (track_mode == TrackMouseMode::NonClientArea &&
            message.ID() == WM_NCMOUSELEAVE) {
            return true;
        }

        return false;
    }();

    if (is_tracking_mouse) {
        window_.lifecycle_facet_->HandleStateData().track_mouse_mode = TrackMouseMode::None;
        SetMouseOverControl(nullptr, MouseMessage{ Message{} });
    }
}


std::shared_ptr<Control> WindowMouseFacet::MouseOverControl() const noexcept {

    auto handle_state = window_.lifecycle_facet_->HandleState();
    if (handle_state == WindowHandleState::Creating ||
        handle_state == WindowHandleState::Created ||
        handle_state == WindowHandleState::Destroying) {
        return window_.lifecycle_facet_->HandleStateData().mouse_over_control;
    }
    return nullptr;
}


void WindowMouseFacet::SetMouseOverControl(
    const std::shared_ptr<Control>& mouse_over_control,
    const MouseMessage& message) {

    auto old_control = this->MouseOverControl();
    if (old_control == mouse_over_control) {
        return;
    }

    if (mouse_over_control != nullptr) {

        //The mouse over control must be contained in this window
        if (mouse_over_control->Window().get() != &window_) {
            return;
        }
    }

    window_.lifecycle_facet_->HandleStateData().mouse_over_control = mouse_over_control;

    if (old_control) {
        ChangeControlMouseOverState(old_control, false, mouse_over_control);
    }

    if (mouse_over_control != nullptr) {

        //Window finds the mouse over control when received WM_MOUSEMOVE,
        //but WM_SETCURSOR is always received before WM_MOUSEMOVE, in such
        //case the mouse over control cannot change the cursor promptly. So 
        //here, a simulated WM_SETCURSOR is posted to give a change to 
        //change the cursor immediately.
        window_.Messager().PostWMSETCURSOR(message);

        ChangeControlMouseOverState(mouse_over_control, true, old_control);

        //Track mouse again to generate next mouse hover message.
        //TODO: Find out whether the new control is in non-client area.
        TrackMouse(false);
    }

    HideTooltipWindow();
}


void WindowMouseFacet::ChangeControlMouseOverState(
    const std::shared_ptr<Control>& target_control,
    bool is_mouse_over,
    const std::shared_ptr<Control>& changed_control) {

    target_control->SetIsMouseOverByWindow(is_mouse_over);

    //Raise and route event.
    auto event_info_state = std::make_shared<MouseOverEventSharedState>(
        target_control,
        changed_control);

    for (auto sender = target_control; sender; sender = sender->Parent()) {

        //Parents at the route path should update their styles to reflect the change of mouse 
        //over state. For example, parents may use ContainsMouse to update their styles.
        sender->NeedUpdateStyle();

        if (is_mouse_over) {
            sender->OnMouseEnter(MouseEnterInfo{ event_info_state, sender });
        }
        else {
            sender->OnMouseLeave(MouseLeaveInfo{ event_info_state, sender });
        }
    }
}


bool WindowMouseFacet::HandleMouseWheelMessage(const Message& message) {

    //Mouse wheel messages are not sent to an unfocused window even if it captures
    //the mouse, because these messages are only sent to focused window.
    //But we wish these messages have the same behaviour as other mouse input messages,
    //so the messages are redirected to the window which is capturing the mouse.
    if (RedirectMouseWheelMessage(message)) {
        return true;
    }

    if (HandleGeneralMouseMessage(MouseWheelMessage{ message })) {
        return true;
    }

    return false;
}


bool WindowMouseFacet::RedirectMouseWheelMessage(const Message& message) {

    HWND capturing_handle = GetCapture();
    if ((capturing_handle == nullptr) || (capturing_handle == window_.Handle())) {
        return false;
    }

    auto capturing_mouse_window = GetWindowFromHandle(capturing_handle);
    if (capturing_mouse_window == nullptr) {
        return false;
    }

    PostMessage(
        capturing_mouse_window->Handle(),
        message.ID(),
        message.WParam(),
        message.LParam());

    return true;
}


bool WindowMouseFacet::RouteMouseEvent(const MouseMessage& message) {

    const auto is_mouse_move = [&message]() {
        return message.ID() == WM_MOUSEMOVE || message.ID() == WM_NCMOUSEMOVE;
    };

    Point position_at_begin_routing_control;
    auto begin_routing_control = GetBeginRoutingControlForMouseMessage(
        message,
        position_at_begin_routing_control);

    auto event_target_info = FindMouseEventTarget(
        begin_routing_control,
        position_at_begin_routing_control);

    if (!event_target_info.control) {
        //Need to clear mouse over control if event target is not found for mouse move message.
        if (is_mouse_move()) {
            SetMouseOverControl(nullptr, MouseMessage{ Message{} });
        }
        return false;
    }

    auto event_info_state = std::make_shared<MouseEventSharedState>(
        event_target_info.control,
        message.Inner(),
        event_target_info.position
    );

    //Tunnel the event.
    TunnelMouseEvent(event_target_info.control, event_info_state, message);
    if (!event_info_state->IsHandled()) {
        //Change mouse over control if the event is not handled in tunneling phase.
        if (is_mouse_move()) {
            SetMouseOverControl(event_target_info.control, message);
        }
    }

    //Bubble the event.
    BubbleMouseEvent(
        event_target_info.control,
        event_target_info.position,
        event_info_state,
        message);

    return event_info_state->IsHandled();
}


std::shared_ptr<Control> WindowMouseFacet::GetBeginRoutingControlForMouseMessage(
    const MouseMessage& message,
    Point& position_at_control) const {

    std::shared_ptr<Control> result;

    auto mouse_capture_control =
        window_.lifecycle_facet_->HandleStateData().mouse_capture_control;

    if (mouse_capture_control) {

        result = mouse_capture_control;

        position_at_control = TranslateAbsolutePositionToControlPosition(
            message.MousePosition(),
            *result);
    }
    else {

        result = window_.root_control_;
        position_at_control = message.MousePosition();
    }

    return result;
}


std::shared_ptr<Control> WindowMouseFacet::MouseCaptureControl() const noexcept {

    auto handle_state = window_.lifecycle_facet_->HandleState();
    if (handle_state == WindowHandleState::Creating ||
        handle_state == WindowHandleState::Created ||
        handle_state == WindowHandleState::Destroying) {
        return window_.lifecycle_facet_->HandleStateData().mouse_over_control;
    }
    return nullptr;
}


void WindowMouseFacet::SetMouseCaptureControl(
    const std::shared_ptr<Control>& capture_control,
    bool is_releasing) {

    if (capture_control == nullptr) {
        return;
    }

    if (!is_releasing) {
        CaptureMouseWithControl(capture_control);
    }
    else {
        ReleaseMouseWithControl(capture_control);
    }
}


void WindowMouseFacet::CaptureMouseWithControl(const std::shared_ptr<Control>& control) {

    auto previous_control = this->MouseCaptureControl();
    if (previous_control == control) {
        return;
    }

    //The control must be contained in this window.
    if (control->Window().get() != &window_) {
        return;
    }

    if (previous_control != nullptr) {
        previous_control->IsCapturingMouseChanged(false);
    }
    else {
        SetCapture(window_.Handle());
    }

    window_.lifecycle_facet_->HandleStateData().mouse_capture_control = control;
    control->IsCapturingMouseChanged(true);

    window_.OnMouseCaptureControlChanged(MouseCaptureControlChangedInfo{
        window_.shared_from_this(),
        previous_control
    });
}


void WindowMouseFacet::ReleaseMouseWithControl(const std::shared_ptr<Control>& control) {

    auto capturing_control = this->MouseCaptureControl();
    if (capturing_control != control) {
        return;
    }

    //ReleaseCapture sends a WM_CAPTURECHANGED message to the window, 
    //in which message handler will set mouse_capture_control_ to nullptr.
    ReleaseCapture();
}


void WindowMouseFacet::CancelMouseCapture() {

    auto capturing_control = this->MouseCaptureControl();
    if (!capturing_control) {
        return;
    }

    //Set mouse_capture_control_ to nullptr before raising events to avoid reentering issues.
    auto previous_control = capturing_control;
    window_.lifecycle_facet_->HandleStateData().mouse_capture_control = nullptr;
    previous_control->IsCapturingMouseChanged(false);

    window_.OnMouseCaptureControlChanged(MouseCaptureControlChangedInfo{
        window_.shared_from_this(),
        previous_control
    });
}


bool WindowMouseFacet::HandleWMSETCURSOR(const Message& message) {

    auto mouse_over_control = this->MouseOverControl();
    if (!mouse_over_control) {
        return false;
    }

    auto event_state = std::make_shared<internal::MouseCursorChangingState>(
        mouse_over_control,
        message);

    for (auto sender = mouse_over_control; sender; sender = sender->Parent()) {
        sender->OnMouseCursorChanging(MouseCursorChangingInfo{ event_state, sender });
    }

    return event_state->IsHandled();
}


void WindowMouseFacet::TryToShowTooltipWindow() {

    auto mouse_over_control = this->MouseOverControl();
    if (!mouse_over_control) {
        return;
    }

    auto tooltip = mouse_over_control->Tooltip();
    if (tooltip.empty()) {
        return;
    }

    auto& tooltip_window = window_.lifecycle_facet_->HandleStateData().tooltip_window;
    if (!tooltip_window) {
        tooltip_window = zaf::Create<TooltipWindow>();
        tooltip_window->SetOwner(window_.shared_from_this());
    }

    tooltip_window->SetText(tooltip);
    tooltip_window->ShowBelowMouseCursor();
}


void WindowMouseFacet::HideTooltipWindow() {

    const auto& tooltip_window = window_.lifecycle_facet_->HandleStateData().tooltip_window;
    if (tooltip_window) {
        tooltip_window->SetText({});
        tooltip_window->Hide();
    }
}

}