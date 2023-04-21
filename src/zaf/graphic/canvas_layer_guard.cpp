#include <zaf/graphic/canvas_layer_guard.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

void CanvasLayerGuard::PopLayer() {
    if (canvas_) {
        canvas_->PopLayer();
    }
}

}