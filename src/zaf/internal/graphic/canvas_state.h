#pragma once

#include <zaf/graphic/d2d/brush.h>
#include <zaf/graphic/d2d/stroke.h>

namespace zaf::internal {

class CanvasState {
public:
    d2d::Brush brush;
    d2d::Stroke stroke;
};

}