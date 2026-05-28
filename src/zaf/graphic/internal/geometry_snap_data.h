#pragma once

#include <zaf/graphic/point.h>

namespace zaf::internal {

class GeometrySnapData {
public:
    float stroke_width{};
    float dpi{};
    Point coordinate_origin;
    Point snapped_coordinate_origin;
};

}