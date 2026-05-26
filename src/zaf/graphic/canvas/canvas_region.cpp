#include <zaf/graphic/canvas/canvas_region.h>
#include <zaf/graphic/canvas/canvas.h>

namespace zaf {

CanvasRegion::CanvasRegion(Canvas* canvas, CanvasClipping&& clipping) noexcept :
    canvas_(canvas),
    clipping_(std::move(clipping)) {

}


CanvasRegion::~CanvasRegion() {
    PopRegion();
}


CanvasRegion::CanvasRegion(CanvasRegion&& other) noexcept :
    canvas_(other.canvas_),
    clipping_(std::move(other.clipping_)) {

    other.canvas_ = nullptr;
}


void CanvasRegion::PopRegion() {
    if (canvas_) {
        canvas_->PopRegion(std::move(clipping_));
    }
}

}