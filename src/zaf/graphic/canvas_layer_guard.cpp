#include <zaf/graphic/canvas_layer_guard.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

CanvasLayerGuard::~CanvasLayerGuard() {
    if (canvas_) {
        canvas_->PopLayer();
    }
}

}