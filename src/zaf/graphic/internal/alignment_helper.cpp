#include <zaf/graphic/internal/alignment_helper.h>
#include <zaf/graphic/alignment.h>

namespace zaf {
namespace internal {

Point AlignWithCoordinateOrigin(const Point& point, const Point& coordinate_origin) {

    Point new_point = point;
    new_point.x += coordinate_origin.x;
    new_point.y += coordinate_origin.y;
    return Align(new_point);
}

}
}