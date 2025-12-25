#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/graphic/rect.h>

namespace zaf {
class Window;
}

namespace zaf::internal {

class WindowRenderFacet : NonCopyableNonMovable {
public:
    explicit WindowRenderFacet(Window& window) noexcept;

    void CreateRenderer();
    void HandleWMPAINT();

    void RequestRepaint() noexcept;
    void RequestRepaint(const zaf::Rect& rect) noexcept;
    void RepaintIfNeeded() noexcept;

private:
    void RecreateRenderer();
    
private:
    Window& window_;
};

}