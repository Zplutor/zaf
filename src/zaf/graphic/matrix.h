#pragma once

#include <zaf/base/direct2d.h>
#include <zaf/graphic/point.h>
#include <zaf/graphic/size.h>

namespace zaf {

class TransformMatrix {
public:
    static TransformMatrix Rotation(float angle, const Point& center_point) {
        auto matrix = D2D1::Matrix3x2F::Rotation(angle, center_point.ToD2D1POINT2F());
        return TransformMatrix(matrix);
    }

    static TransformMatrix Scale(const Size& size, const Point& center_point) {
        auto matrix = D2D1::Matrix3x2F::Scale(size.ToD2D1SIZEF(), center_point.ToD2D1POINT2F());
        return TransformMatrix(matrix);
    }

    static TransformMatrix Skew(float x_angle, float y_angle, const Point& center_point) {
        auto matrix = D2D1::Matrix3x2F::Skew(x_angle, y_angle, center_point.ToD2D1POINT2F());
        return TransformMatrix(matrix);
    }

    static TransformMatrix Translation(const Point& point) {
        auto matrix = D2D1::Matrix3x2F::Translation(point.x, point.y);
        return TransformMatrix(matrix);
    }

    static TransformMatrix FromD2D1MATRIX3X2F(const D2D1::Matrix3x2F& matrix) {
        return TransformMatrix(matrix);
    }

    static const TransformMatrix Identity;

public:
    TransformMatrix() { }

    const D2D1::Matrix3x2F& ToD2D1MATRIX3X2F() const {
        return matrix_;
    }

    Point TransformPoint(const Point& point) const;

private:
    explicit TransformMatrix(const D2D1::Matrix3x2F& martix) : matrix_(martix) { }

private:
    D2D1::Matrix3x2F matrix_;
};

}