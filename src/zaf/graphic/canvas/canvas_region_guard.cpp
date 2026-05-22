#include <zaf/graphic/canvas/canvas_region_guard.h>
#include <zaf/graphic/canvas/canvas.h>

namespace zaf {

void CanvasRegionGuard::PopRegion() {
    if (canvas_) {
        canvas_->PopRegion(std::move(clipping_guard_));
    }
}

}