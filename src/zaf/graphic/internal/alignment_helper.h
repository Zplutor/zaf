#pragma once

#include <zaf/graphic/point.h>

namespace zaf {
namespace internal {

Point AlignWithCoordinateOrigin(const Point& point, const Point& coordinate_origin);

}
}