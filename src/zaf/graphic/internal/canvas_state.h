#pragma once

#include <zaf/graphic/brush/brush.h>
#include <zaf/graphic/stroke.h>

namespace zaf::internal {

class CanvasState {
public:
    zaf::Brush brush;
    zaf::Stroke stroke;
};

}