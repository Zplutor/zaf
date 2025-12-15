#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/control.h>
#include <zaf/internal/window/window_focused_control_manager.h>
#include <zaf/window/window.h>

namespace zaf::internal {

class WindowFocusFacet : NonCopyableNonMovable {
public:
    explicit WindowFocusFacet(Window& window) noexcept;

    void HandleWMACTIVATE(const ActivateMessage& message);
    LRESULT HandleWMMOUSEACTIVATE();

    bool IsFocused() const noexcept;
    void HandleWMSETFOCUS(const Message& message);
    void HandleWMKILLFOCUS(const Message& message);

    bool TryToPreprocessTabKeyMessage(const KeyMessage& message);

    void ChangeFocusedControl(const std::shared_ptr<Control>& new_focused_control);
    std::shared_ptr<Control> FocusedControl() const noexcept;

    void HandleWindowDestroy();

private:
    Window& window_;
    WindowFocusedControlManager focused_control_manager_;
};

}