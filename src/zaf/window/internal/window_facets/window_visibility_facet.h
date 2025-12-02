#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/window/show_options.h>

namespace zaf {
class Window;
}

namespace zaf::internal {

class WindowVisibilityFacet : public NonCopyableNonMovable {
public:
    explicit WindowVisibilityFacet(Window& window) noexcept;

    void Show(ShowOptions options);
    void Maximize();
    void Minimize();
    void Restore();
    void Hide() noexcept;

    bool IsVisible() const noexcept;
    bool IsWindowMaximized() const noexcept;
    bool IsWindowMinimized() const noexcept;

private:
    void InnerShowWindow(int show_command);

private:
    Window& window_;
};

}