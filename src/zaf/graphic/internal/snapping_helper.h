#pragma once

#include <zaf/graphic/pixel_snapping.h>

namespace zaf::internal {

template<typename T>
T SnapInCoordinateSystem(
    const T& object,
    float stroke_width,
    float dpi,
    const Point& coordinate_origin,
    const Point& snapped_coordinate_origin) {

    auto result = object;

    // Transform the object to the global coordinate system.
    result.AddOffset(coordinate_origin);

    // Snap the object in global coordinate system.
    result = SnapToPixels(result, stroke_width, dpi);

    // Transform the snapped object back to the coordinate system, using the snapped coordinate
    // origin.
    result.SubtractOffset(snapped_coordinate_origin);

    // Adjust the object by the offset of the snapped coordinate origin to the original coordinate
    // origin, as the canvas is transformed by the coordinate origin but not the snapped coordinate
    // origin.
    auto snapped_offset = snapped_coordinate_origin - coordinate_origin;
    result.AddOffset(snapped_offset);

    return result;
}

}