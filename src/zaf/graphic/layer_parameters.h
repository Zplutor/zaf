#pragma once

#include <zaf/graphic/geometry/geometry.h>
#include <zaf/graphic/matrix.h>
#include <zaf/graphic/rect.h>

namespace zaf {

class LayerParameters {
public:
    LayerParameters() :
        content_bounds(Rect::Infinite),
        mask_transform(TransformMatrix::Identity),
        opacity(1) {

    }

    Rect content_bounds;
    Geometry geometric_mask;
    TransformMatrix mask_transform;
    float opacity;
};

}