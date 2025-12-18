#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf {
class Window;
}

namespace zaf::internal {

class WindowRenderFacet : NonCopyableNonMovable {
public:
    explicit WindowRenderFacet(Window& window) noexcept;

    void CreateRenderer();
    void HandleWMPAINT();

private:
    void RecreateRenderer();
    
private:
    Window& window_;
};

}