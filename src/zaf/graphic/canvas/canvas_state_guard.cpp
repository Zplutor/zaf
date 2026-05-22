#include <zaf/graphic/canvas/canvas_state_guard.h>
#include <zaf/graphic/canvas/canvas.h>

namespace zaf {

void CanvasStateGuard::PopState() {
    if (canvas_) {
        canvas_->PopState(tag_);
    }
}

}