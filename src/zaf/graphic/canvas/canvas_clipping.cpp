#include <zaf/graphic/canvas/canvas_clipping.h>
#include <zaf/graphic/canvas/canvas.h>

namespace zaf {

CanvasClipping::CanvasClipping(Canvas* canvas, std::size_t tag) noexcept :
    canvas_(canvas),
    tag_(tag) {

}


CanvasClipping::~CanvasClipping() {
    PopClipping();
}


CanvasClipping::CanvasClipping(CanvasClipping&& other) noexcept :
    canvas_(other.canvas_), 
    tag_(other.tag_) {

    other.canvas_ = nullptr;
    other.tag_ = 0;
}


void CanvasClipping::PopClipping() {
    if (canvas_) {
        canvas_->PopClipping(tag_);
    }
}

}