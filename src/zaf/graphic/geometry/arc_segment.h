#pragma once

#include <zaf/graphic/point.h>

namespace zaf {

enum class SweepDirection {
    CounterClockwise = D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
    Clockwise = D2D1_SWEEP_DIRECTION_CLOCKWISE,
};


enum class ArcSize {
    Small = D2D1_ARC_SIZE_SMALL,
    Large = D2D1_ARC_SIZE_LARGE,
};


class ArcSegment {
public:
    ArcSegment() {

        SetSweepDirection(SweepDirection::Clockwise);
    }

    Point EndPoint() const {
        return Point::FromD2D1POINT2F(value_.point);
    }

    void SetEndPoint(const Point& point) {
        value_.point = point.ToD2D1POINT2F();
    }

    float XRadius() const {
        return value_.size.width;
    }

    void SetXRadius(float value) {
        value_.size.width = value;
    }

    float YRadius() const {
        return value_.size.height;
    }

    void SetYRadius(float value) {
        value_.size.height = value;
    }

    float RotationAngle() const {
        return value_.rotationAngle;
    }

    void SetRotationAngle(float value) {
        value_.rotationAngle = value;
    }

    SweepDirection SweepDirection() const {
        return static_cast<zaf::SweepDirection>(value_.sweepDirection);
    }

    void SetSweepDirection(zaf::SweepDirection value) {
        value_.sweepDirection = static_cast<D2D1_SWEEP_DIRECTION>(value);
    }

    ArcSize ArcSize() const {
        return static_cast<zaf::ArcSize>(value_.arcSize);
    }

    void SetArcSize(zaf::ArcSize value) {
        value_.arcSize = static_cast<D2D1_ARC_SIZE>(value);
    }

    const D2D1_ARC_SEGMENT& Inner() const {
        return value_;
    }

private:
    D2D1_ARC_SEGMENT value_{};
};

}