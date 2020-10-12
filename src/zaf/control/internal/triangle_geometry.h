#pragma once

#include <zaf/graphic/geometry/geometry.h>

namespace zaf::internal {

Geometry CreateTriangleGeometry(
    float canvas_width, 
    float canvas_height, 
    float bottom_edge_length,
    float rotate_angle);

}