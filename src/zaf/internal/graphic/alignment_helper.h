#pragma once

#include <zaf/graphic/alignment.h>

namespace zaf {
namespace internal {

template<typename T>
T AlignInRelatedCoordinateSystem(
    const T& object,
    float stroke_width,
    float dpi,
    const Point& coordinate_origin,
    const Point& aligned_coordinate_origin) {

    auto result = object;
    result.AddOffset(coordinate_origin);
    result = ToPixelAligned(result, stroke_width, dpi);
    result.SubtractOffset(aligned_coordinate_origin);
    return result;
}

}
}