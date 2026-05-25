#pragma once

#include <vector>
#include <zaf/graphic/d2d/dash_style.h>
#include <zaf/graphic/d2d/line_cap.h>
#include <zaf/graphic/d2d/line_join.h>

namespace zaf::d2d {

class StrokeStyleProperties {
public:
    LineCap StartCap() const noexcept {
        return static_cast<LineCap>(value_.startCap);
    }

    void SetStartCap(LineCap value) noexcept {
        value_.startCap = static_cast<D2D1_CAP_STYLE>(value);
    }

    LineCap EndCap() const noexcept {
        return static_cast<LineCap>(value_.endCap);
    }

    void SetEndCap(LineCap value) noexcept {
        value_.endCap = static_cast<D2D1_CAP_STYLE>(value);
    }

    LineCap DashCap() const noexcept {
        return static_cast<LineCap>(value_.dashCap);
    }

    void SetDashCap(LineCap value) noexcept {
        value_.dashCap = static_cast<D2D1_CAP_STYLE>(value);
    }

    d2d::LineJoin GetLineJoin() const noexcept {
        return static_cast<d2d::LineJoin>(value_.lineJoin);
    }

    void SetLineJoin(d2d::LineJoin value) noexcept {
        value_.lineJoin = static_cast<D2D1_LINE_JOIN>(value);
    }

    float MiterLimit() const noexcept {
        return value_.miterLimit;
    }

    void SetMiterLimit(float value) noexcept {
        value_.miterLimit = value;
    }

    d2d::DashStyle GetDashStyle() const noexcept {
        return static_cast<d2d::DashStyle>(value_.dashStyle);
    }

    void SetDashStyle(d2d::DashStyle value) noexcept {
        value_.dashStyle = static_cast<D2D1_DASH_STYLE>(value);
    }

    float DashOffset() const noexcept {
        return value_.dashOffset;
    }

    void SetDashOffset(float value) noexcept {
        value_.dashOffset = value;
    }

    const std::vector<float>& DashPattern() const noexcept {
        return dash_pattern_;
    }

    void SetDashPattern(std::vector<float> value) noexcept {
        dash_pattern_ = std::move(value);
    }

    const D2D1_STROKE_STYLE_PROPERTIES& Inner() const noexcept {
        return value_;
    }

private:
    D2D1_STROKE_STYLE_PROPERTIES value_{};
    std::vector<float> dash_pattern_;
};

}