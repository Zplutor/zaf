#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf {

class Canvas;

class CanvasStateGuard : NonCopyable {
public:
    CanvasStateGuard() : canvas_(nullptr), tag_(0) {

    }

    CanvasStateGuard(Canvas* canvas, std::size_t tag) : canvas_(canvas), tag_(tag) {

    }

    ~CanvasStateGuard() {
        PopState();
    }

    CanvasStateGuard(CanvasStateGuard&& other) : canvas_(other.canvas_), tag_(other.tag_) {
        other.canvas_ = nullptr;
        other.tag_ = 0;
    }

    CanvasStateGuard& operator=(CanvasStateGuard&& other) {
        PopState();
        canvas_ = other.canvas_;
        tag_ = other.tag_;
        other.canvas_ = nullptr;
        other.tag_ = 0;
        return *this;
    }

private:
    void PopState();

private:
    Canvas* canvas_;
    std::size_t tag_;
};

}