#include <zaf/graphic/canvas_state_guard.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

void CanvasStateGuard::PopState() {
    if (canvas_) {
        canvas_->PopState(tag_);
    }
}

}