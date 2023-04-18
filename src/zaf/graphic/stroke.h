#pragma once

#include <vector>
#include <zaf/base/com_object.h>
#include <zaf/base/direct2d.h>

namespace zaf {

class Stroke : public COMObject<ID2D1StrokeStyle> {
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
    explicit Stroke(ID2D1StrokeStyle* handle) : COMObject(handle) { }

    CapStyle GetStartCapStyle() const {
        return static_cast<CapStyle>(Inner()->GetStartCap());
    }

    CapStyle GetEndCapStyle() const {
        return static_cast<CapStyle>(Inner()->GetEndCap());
    }

    CapStyle GetDashCapStyle() const {
        return static_cast<CapStyle>(Inner()->GetDashCap());
    }

    LineJoinStyle GetLineJoinStyle() const {
        return static_cast<LineJoinStyle>(Inner()->GetLineJoin());
    }

    float GetMiterLimit() const {
        return Inner()->GetMiterLimit();
    }

    DashStyle GetDashStyle() const {
        return static_cast<DashStyle>(Inner()->GetDashStyle());
    }

    float GetDashOffset() const {
        return Inner()->GetDashOffset();
    }

    std::vector<float> GetDashPattern() const {

        auto count = Inner()->GetDashesCount();

        std::vector<float> dash_pattern(count);
        Inner()->GetDashes(dash_pattern.data(), count);
        return dash_pattern;
    }
};

}