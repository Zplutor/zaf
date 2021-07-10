#include <zaf/graphic/matrix.h>

namespace zaf {

const TransformMatrix TransformMatrix::Identity(D2D1::Matrix3x2F::Identity());


Point TransformMatrix::TransformPoint(const Point& point) const {
    return Point::FromD2D1POINT2F(matrix_.TransformPoint(point.ToD2D1POINT2F()));
}

}
