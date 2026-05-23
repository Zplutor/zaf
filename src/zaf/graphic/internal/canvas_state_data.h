#pragma once

#include <zaf/graphic/canvas/pixel_snap_mode.h>
#include <zaf/graphic/d2d/brush.h>
#include <zaf/graphic/d2d/stroke.h>

namespace zaf::internal {

class CanvasStateData {
public:
    PixelSnapMode pixel_snap_mode{ PixelSnapMode::Snap };
    d2d::Brush brush;
    d2d::Stroke stroke;
};

}