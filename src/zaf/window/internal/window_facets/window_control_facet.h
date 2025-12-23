#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf {
class Control;
class Window;
}

namespace zaf::internal {

class WindowControlFacet : NonCopyableNonMovable {
public:
    explicit WindowControlFacet(Window& window) noexcept;

    void OnWindowInitialize();

    const std::shared_ptr<Control>& RootControl() const noexcept;
    void SetRootControl(const std::shared_ptr<Control>& control);

private:
    std::shared_ptr<Control> UninstallRootControl();
    void InstallRootControl(std::shared_ptr<Control> control);

private:
    Window& window_;
    std::shared_ptr<Control> root_control_;
};

}