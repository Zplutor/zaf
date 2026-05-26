#pragma once

/**
@file
    Defines the `zaf::CanvasClipping` class.
*/

#include <zaf/base/non_copyable.h>

namespace zaf {

class Canvas;

/**
Represents a rectangle area that limits the painting area of a canvas.
*/
class CanvasClipping : NonCopyable {
public:
    ~CanvasClipping();

    CanvasClipping(CanvasClipping&& other) noexcept;
    CanvasClipping& operator=(CanvasClipping&& other) noexcept = delete;

private:
    CanvasClipping(Canvas* canvas, std::size_t tag) noexcept;
    void PopClipping();

private:
    Canvas* canvas_{};
    std::size_t tag_{};

    friend Canvas;
};

}