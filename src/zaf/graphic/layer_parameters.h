#pragma once

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
    TransformMatrix mask_transform;
    float opacity;
};

}