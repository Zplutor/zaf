#pragma once

#include <vector>
#include <zaf/base/com_object.h>
#include <zaf/base/direct2d.h>

namespace zaf {

class Stroke : public ComObject<ID2D1StrokeStyle> {
public:
    enum class CapStyle {
        Flat = 0,
        Square = 1,
        Round = 2,
        Triangle = 3,
    };

    enum class LineJoinStyle {
        Miter = 0,
        Bevel = 1,
        Round = 2,
        MiterOrBevel = 3,
    };

    enum class DashStyle {
        Solid = 0,
        Dash = 1,
        Dot = 2,
        DashDot = 3,
        DashDotDot = 4,
        Custom = 5,
    };

public:
    Stroke() { }
    explicit Stroke(ID2D1StrokeStyle* handle) : ComObject(handle) { }

    CapStyle GetStartCapStyle() const {
        return static_cast<CapStyle>(GetHandle()->GetStartCap());
    }

    CapStyle GetEndCapStyle() const {
        return static_cast<CapStyle>(GetHandle()->GetEndCap());
    }

    CapStyle GetDashCapStyle() const {
        return static_cast<CapStyle>(GetHandle()->GetDashCap());
    }

    LineJoinStyle GetLineJoinStyle() const {
        return static_cast<LineJoinStyle>(GetHandle()->GetLineJoin());
    }

    float GetMiterLimit() const {
        return GetHandle()->GetMiterLimit();
    }

    DashStyle GetDashStyle() const {
        return static_cast<DashStyle>(GetHandle()->GetDashStyle());
    }

    float GetDashOffset() const {
        return GetHandle()->GetDashOffset();
    }

    std::vector<float> GetDashPattern() const {

        auto count = GetHandle()->GetDashesCount();

        std::vector<float> dash_pattern(count);
        GetHandle()->GetDashes(dash_pattern.data(), count);
        return dash_pattern;
    }
};

}