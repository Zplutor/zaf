#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf {

class Canvas;

class CanvasClippingGuard : NonCopyable {
public:
    CanvasClippingGuard() : canvas_(nullptr), tag_(0) {

    }

    CanvasClippingGuard(Canvas* canvas, std::size_t tag) : canvas_(canvas), tag_(tag) {

    }

    ~CanvasClippingGuard() {
        PopClipping();
    }

    CanvasClippingGuard(CanvasClippingGuard&& other) : canvas_(other.canvas_), tag_(other.tag_) {
        other.canvas_ = nullptr;
        other.tag_ = 0;
    }

    CanvasClippingGuard& operator=(CanvasClippingGuard&& other) {
        PopClipping();
        canvas_ = other.canvas_;
        tag_ = other.tag_;
        other.canvas_ = nullptr;
        other.tag_ = 0;
        return *this;
    }

private:
    void PopClipping();

private:
    Canvas* canvas_;
    std::size_t tag_;
};

}