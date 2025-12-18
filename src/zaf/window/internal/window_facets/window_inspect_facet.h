#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/graphic/rect.h>

namespace zaf {
class Canvas;
class Control;
class Window;
}

namespace zaf::internal {

class InspectorPort;

class WindowInspectFacet : NonCopyableNonMovable {
public:
    explicit WindowInspectFacet(Window& window) noexcept;

    void ShowInspector() const;
    void BeginSelectInspectedControl();
    bool IsSelectingInspectedControl() const noexcept;
    void SelectInspectedControl();
    void SetHighlightControl(const std::shared_ptr<Control>& control);
    void HighlightControlAtPosition(const Point& position);
    std::shared_ptr<InspectorPort> GetInspectorPort() const noexcept;
    void PaintInspectedControl(Canvas& canvas, const Rect& dirty_rect);

private:
    Window& window_;
};

}