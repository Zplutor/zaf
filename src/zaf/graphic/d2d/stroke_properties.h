#pragma once

#include <zaf/graphic/d2d/stroke.h>

namespace zaf::d2d {

class StrokeProperties {
public:
    Stroke::CapStyle StartCapStyle() const {
        return static_cast<Stroke::CapStyle>(value_.startCap);
    }

    void SetStartCapStyle(Stroke::CapStyle value) {
        value_.startCap = static_cast<D2D1_CAP_STYLE>(value);
    }

    Stroke::CapStyle EndCapStyle() const {
        return static_cast<Stroke::CapStyle>(value_.endCap);
    }

    void SetEndCapStyle(Stroke::CapStyle value) {
        value_.endCap = static_cast<D2D1_CAP_STYLE>(value);
    }

    Stroke::CapStyle DashCapStyle() const {
        return static_cast<Stroke::CapStyle>(value_.dashCap);
    }

    void SetDashCapStyle(Stroke::CapStyle value) {
        value_.dashCap = static_cast<D2D1_CAP_STYLE>(value);
    }

    Stroke::LineJoinStyle LineJoinStyle() const {
        return static_cast<Stroke::LineJoinStyle>(value_.lineJoin);
    }

    void SetLineJoinStyle(Stroke::LineJoinStyle value) {
        value_.lineJoin = static_cast<D2D1_LINE_JOIN>(value);
    }

    float MiterLimit() const {
        return value_.miterLimit;
    }

    void SetMiterLimit(float value) {
        value_.miterLimit = value;
    }

    Stroke::DashStyle DashStyle() const {
        return static_cast<Stroke::DashStyle>(value_.dashStyle);
    }

    void SetDashStyle(Stroke::DashStyle value) {
        value_.dashStyle = static_cast<D2D1_DASH_STYLE>(value);
    }

    float DashOffset() const {
        return value_.dashOffset;
    }

    void SetDashOffset(float value) {
        value_.dashOffset = value;
    }

    const std::vector<float>& DashPattern() const {
        return dash_pattern_;
    }

    void SetDashPattern(const std::vector<float>& value) {
        dash_pattern_ = value;
    }

    const D2D1_STROKE_STYLE_PROPERTIES& Inner() const {
        return value_;
    }

private:
    D2D1_STROKE_STYLE_PROPERTIES value_{};
    std::vector<float> dash_pattern_;
};

}