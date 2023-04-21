#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf {

class Canvas;

class CanvasLayerGuard : NonCopyable {
public:
    explicit CanvasLayerGuard(Canvas* canvas) : canvas_(canvas) {

    }

    ~CanvasLayerGuard();

    CanvasLayerGuard(CanvasLayerGuard&& other) : canvas_(other.canvas_) {
        other.canvas_ = nullptr;
    }

    CanvasLayerGuard& operator=(CanvasLayerGuard&& other) {
        canvas_ = other.canvas_;
        other.canvas_ = nullptr;
        return *this;
    }

private:
    Canvas* canvas_;
};

}