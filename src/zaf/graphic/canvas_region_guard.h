#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf {

class Canvas;

class CanvasRegionGuard : NonCopyable {
public:
    CanvasRegionGuard() : canvas_(nullptr) {

    }

    explicit CanvasRegionGuard(Canvas* canvas) : canvas_(canvas) {

    }

    ~CanvasRegionGuard() {
        PopRegion();
    }

    CanvasRegionGuard(CanvasRegionGuard&& other) : canvas_(other.canvas_) {
        other.canvas_ = nullptr;
    }

    CanvasRegionGuard& operator=(CanvasRegionGuard&& other) {
        PopRegion();
        canvas_ = other.canvas_;
        other.canvas_ = nullptr;
        return *this;
    }

private:
    void PopRegion();

private:
    Canvas* canvas_;
};

}