#include <zaf/graphic/canvas_clipping_guard.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

void CanvasClippingGuard::PopClipping() {
    if (canvas_) {
        canvas_->PopClipping(tag_);
    }
}

}