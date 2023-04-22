#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/graphic/canvas_clipping_guard.h>

namespace zaf {

class Canvas;

class CanvasRegionGuard : NonCopyable {
public:
    CanvasRegionGuard() : canvas_(nullptr) {

    }

    CanvasRegionGuard(Canvas* canvas, CanvasClippingGuard&& clipping_guard) : 
        canvas_(canvas),
        clipping_guard_(std::move(clipping_guard)) {

    }

    ~CanvasRegionGuard() {
        PopRegion();
    }

    CanvasRegionGuard(CanvasRegionGuard&& other) : 
        canvas_(other.canvas_), 
        clipping_guard_(std::move(other.clipping_guard_)) {

        other.canvas_ = nullptr;
    }

    CanvasRegionGuard& operator=(CanvasRegionGuard&& other) {
        PopRegion();
        canvas_ = other.canvas_;
        clipping_guard_ = std::move(other.clipping_guard_);
        other.canvas_ = nullptr;
        return *this;
    }

private:
    void PopRegion();

private:
    Canvas* canvas_;
    CanvasClippingGuard clipping_guard_;
};

}