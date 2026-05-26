#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/graphic/canvas/pixel_snap_mode.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/d2d/brush.h>
#include <zaf/graphic/d2d/stroke_style.h>

namespace zaf::internal {
class CanvasStateData;
}

namespace zaf {

class Canvas;

/**
Contains options and graphic resources used for painting on canvas.
*/
class CanvasState : NonCopyable {
public:
    ~CanvasState();

    CanvasState(CanvasState&& other) noexcept;

    // Not allow to be assigned.
    CanvasState& operator=(CanvasState&& other) noexcept = delete;

    void SetPixelSnapMode(PixelSnapMode pixel_snap_mode);
    void SetBrush(d2d::Brush brush);
    void SetBrush(const Color& color);
    void SetStrokeWidth(float stroke_width);
    void SetStrokeStyle(d2d::StrokeStyle stroke_style);

private:
    CanvasState(Canvas* canvas, internal::CanvasStateData* data, std::size_t tag) noexcept;
    void PopState();

private:
    Canvas* canvas_{};
    internal::CanvasStateData* data_{};
    std::size_t tag_{};

    friend Canvas;
};

}