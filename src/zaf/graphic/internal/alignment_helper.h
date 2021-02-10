#pragma once

#include <zaf/graphic/alignment.h>

namespace zaf {
namespace internal {

template<typename T>
T AlignInRelatedCoordinateSystem(
    const T& object,
    float stroke_width,
    const Point& coordinate_origin,
    const Point& aligned_coordinate_origin) {

    auto result = object;
    result.AddOffset(coordinate_origin);
    result = Align(result, stroke_width);
    result.SubtractOffset(aligned_coordinate_origin);
    return result;
}

}
}