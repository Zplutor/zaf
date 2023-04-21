#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf {

class Canvas;

class CanvasLayerGuard : NonCopyable {
public:
    CanvasLayerGuard() : canvas_(nullptr) {

    }

    explicit CanvasLayerGuard(Canvas* canvas) : canvas_(canvas) {

    }

    ~CanvasLayerGuard() {
        PopLayer();
    }

    CanvasLayerGuard(CanvasLayerGuard&& other) : canvas_(other.canvas_) {
        other.canvas_ = nullptr;
    }

    CanvasLayerGuard& operator=(CanvasLayerGuard&& other) {
        PopLayer();
        canvas_ = other.canvas_;
        other.canvas_ = nullptr;
        return *this;
    }

private:
    void PopLayer();

private:
    Canvas* canvas_;
};

}