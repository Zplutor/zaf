#include <zaf/graphic/canvas_region_guard.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

void CanvasRegionGuard::PopRegion() {
    if (canvas_) {
        canvas_->PopRegion();
    }
}

}