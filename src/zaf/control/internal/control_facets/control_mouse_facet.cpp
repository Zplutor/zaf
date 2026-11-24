#include <zaf/control/internal/control_facets/control_mouse_facet.h>
#include <Windows.h>
#include <zaf/control/control.h>
#include <zaf/window/internal/window_facets/window_mouse_facet.h>
#include <zaf/window/window.h>

namespace zaf::internal {

ControlMouseFacet::ControlMouseFacet(Control& control) noexcept : control_(control) {

}


bool ControlMouseFacet::IsMouseOver() const noexcept {
    return is_mouse_over_;
}


void ControlMouseFacet::SetIsMouseOver(bool is_mouse_over) {
    is_mouse_over_ = is_mouse_over;
    control_.NeedUpdateStyle();
    control_.NeedRepaint();
}


bool ControlMouseFacet::ContainsMouse() const noexcept {

    if (IsMouseOver()) {
        return true;
    }

    auto window = control_.Window();
    if (!window) {
        return false;
    }

    const auto& mouse_over_control = window->MouseOverControl();
    if (!mouse_over_control) {
        return false;
    }

    return control_.IsAncestorOf(*mouse_over_control);
}


bool ControlMouseFacet::CanDoubleClick() const noexcept {
    return can_double_click_;
}


void ControlMouseFacet::SetCanDoubleClick(bool can_double_click) noexcept {
    can_double_click_ = can_double_click;
}


void ControlMouseFacet::OnMouseDown(const MouseDownInfo& event_info) {

    control_.mouse_down_event_.Raise(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    if (event_info.Message().MouseButton() == MouseButton::Left) {
        if (HandleDoubleClickOnMouseDown(event_info.PositionAtSender())) {
            event_info.MarkAsHandled();
        }
    }
}


bool ControlMouseFacet::HandleDoubleClickOnMouseDown(const Point& position) {

    if (!CanDoubleClick()) {
        return false;
    }

    //Check if need to raise double click.
    std::uint32_t current_time = GetTickCount();
    if (current_time - last_mouse_down_time_ <= GetDoubleClickTime()) {

        if (last_mouse_down_position_ == position) {
            last_mouse_down_time_ = 0;
            RaiseDoubleClickEvent(position);
            return true;
        }
    }

    last_mouse_down_time_ = current_time;
    last_mouse_down_position_ = position;
    return false;
}


void ControlMouseFacet::RaiseDoubleClickEvent(const Point& position) {
    DoubleClickInfo event_info{ control_.shared_from_this(), position };
    control_.OnDoubleClick(event_info);
}


bool ControlMouseFacet::CaptureMouse() {

    auto shared_this = control_.weak_from_this().lock();
    if (!shared_this) {
        return false;
    }

    auto window = control_.Window();
    if (!window) {
        return false;
    }

    return window->mouse_facet_->CaptureMouseWithControl(shared_this);
}


void ControlMouseFacet::ReleaseMouse() {

    auto shared_this = control_.weak_from_this().lock();
    if (!shared_this) {
        return;
    }

    auto window = control_.Window();
    if (!window) {
        return;
    }

    window->mouse_facet_->ReleaseMouseWithControl(shared_this);
}


bool ControlMouseFacet::HasMouseCapture() const noexcept {
    auto window = control_.Window();
    if (window) {
        return window->MouseCaptureControl().get() == &control_;
    }
    return false;
}


const Point ControlMouseFacet::GetMousePosition() const {

    auto window = control_.Window();
    if (window == nullptr) {
        return Point();
    }

    Point mouse_position = window->MousePosition();
    Point absolute_position = *control_.PositionInWindow();
    mouse_position.x -= absolute_position.x;
    mouse_position.y -= absolute_position.y;
    return mouse_position;
}


void ControlMouseFacet::IsCapturingMouseChanged(bool is_capturing_mouse) {

    MouseCaptureEventInfo event_info{ control_.shared_from_this() };
    if (is_capturing_mouse) {
        control_.OnMouseCaptured(event_info);
    }
    else {
        control_.OnMouseReleased(event_info);
    }
}


std::optional<HitTestResult> ControlMouseFacet::HitTest(const HitTestMessage& message) {

    auto parent = control_.Parent();
    if (parent != nullptr) {
        return parent->HitTest(message);
    }
    return {};
}

}
