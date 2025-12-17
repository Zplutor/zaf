#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/window/message/hit_test_message.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {
class Control;
class Window;
}

namespace zaf::internal {

class WindowMouseFacet : NonCopyableNonMovable {
public:
    explicit WindowMouseFacet(Window& window) noexcept;

    std::optional<HitTestResult> HitTest(const HitTestMessage& message);

    bool HandleGeneralMouseMessage(const MouseMessage& message);
    void HandleMouseHoverMessage(const Message& message);
    void HandleMouseLeaveMessage(const Message& message);
    bool HandleMouseWheelMessage(const Message& message);

    std::shared_ptr<Control> MouseOverControl() const noexcept;
    void SetMouseOverControl(
        const std::shared_ptr<Control>& mouse_over_control,
        const MouseMessage& message);

    std::shared_ptr<Control> MouseCaptureControl() const noexcept;
    void SetMouseCaptureControl(
        const std::shared_ptr<Control>& capture_control,
        bool is_releasing);
    void CancelMouseCapture();

    bool HandleWMSETCURSOR(const Message& message);

private:
    void TrackMouseByMouseMove(const MouseMessage& message);
    void TrackMouse(bool is_non_client);
    
    void ChangeControlMouseOverState(
        const std::shared_ptr<Control>& target_control,
        bool is_mouse_over,
        const std::shared_ptr<Control>& changed_control);

    bool RedirectMouseWheelMessage(const Message& message);

    bool RouteMouseEvent(const MouseMessage& message);
    std::shared_ptr<Control> GetBeginRoutingControlForMouseMessage(
        const MouseMessage& message,
        Point& position_at_control) const;

    void CaptureMouseWithControl(const std::shared_ptr<Control>& control);
    void ReleaseMouseWithControl(const std::shared_ptr<Control>& control);

    void TryToShowTooltipWindow();
    void HideTooltipWindow();

private:
    Window& window_;
};

}