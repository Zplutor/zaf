#include <zaf/graphic/canvas/canvas_clipping_guard.h>
#include <zaf/graphic/canvas/canvas.h>

namespace zaf {

void CanvasClippingGuard::PopClipping() {
    if (canvas_) {
        canvas_->PopClipping(tag_);
    }
}

}