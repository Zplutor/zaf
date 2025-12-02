#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf {
class Window;
}

namespace zaf::internal {

class WindowVisibilityFacet : public NonCopyableNonMovable {
public:
    explicit WindowVisibilityFacet(Window& window) noexcept;

private:
    Window& window_;
};

}