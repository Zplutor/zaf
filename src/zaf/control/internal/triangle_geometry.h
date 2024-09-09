#pragma once

#include <zaf/graphic/d2d/geometry.h>

namespace zaf::internal {

d2d::Geometry CreateTriangleGeometry(
    float canvas_width, 
    float canvas_height, 
    float bottom_edge_length,
    float rotate_angle);

}