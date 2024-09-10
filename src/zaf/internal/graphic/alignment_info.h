#pragma once

#include <zaf/graphic/point.h>

namespace zaf::internal {

class AlignmentInfo {
public:
    Point coordinate_origin;
    Point aligned_coordinate_origin;
    float dpi{};
};

}