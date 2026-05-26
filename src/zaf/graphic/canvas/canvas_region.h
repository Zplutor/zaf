#pragma once

/**
@file
    Defines the `zaf::CanvasRegion` class.
*/

#include <zaf/base/non_copyable.h>
#include <zaf/graphic/canvas/canvas_clipping.h>

namespace zaf {

class Canvas;

/**
Represents a rectangle area that transforms the coordinate origin and limits the painting area
on canvas.

@see zaf::Canvas
*/
class CanvasRegion : NonCopyable {
public:
    ~CanvasRegion();

    CanvasRegion(CanvasRegion&& other) noexcept;
    CanvasRegion& operator=(CanvasRegion&& other) noexcept = delete;

private:
    CanvasRegion(Canvas* canvas, CanvasClipping&& clipping) noexcept;
    void PopRegion();

private:
    Canvas* canvas_{};
    CanvasClipping clipping_;

    friend Canvas;
};

}