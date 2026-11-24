#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <zaf/base/non_copyable.h>
#include <zaf/control/event/mouse_event_info.h>
#include <zaf/graphic/point.h>
#include <zaf/window/message/hit_test_message.h>
#include <zaf/window/message/hit_test_result.h>

namespace zaf {
class Control;
}

namespace zaf::internal {

class ControlMouseFacet : NonCopyableNonMovable {
public:
    explicit ControlMouseFacet(Control& control) noexcept;

    bool IsMouseOver() const noexcept;
    void SetIsMouseOver(bool is_mouse_over);

    bool ContainsMouse() const noexcept;

    bool CanDoubleClick() const noexcept;
    void SetCanDoubleClick(bool can_double_click) noexcept;
    void OnMouseDown(const MouseDownInfo& event_info);

    bool CaptureMouse();
    void ReleaseMouse();

    bool HasMouseCapture() const noexcept;
    const Point GetMousePosition() const;

    void IsCapturingMouseChanged(bool is_capturing_mouse);

    std::optional<HitTestResult> HitTest(const HitTestMessage& message);

private:
    bool HandleDoubleClickOnMouseDown(const Point& position);
    void RaiseDoubleClickEvent(const Point& position);

private:
    Control& control_;

    bool is_mouse_over_{};
    bool can_double_click_{};
    std::uint32_t last_mouse_down_time_{};
    Point last_mouse_down_position_;
};

}
