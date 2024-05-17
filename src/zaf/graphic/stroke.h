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
    using COMObject::COMObject;

    CapStyle GetStartCapStyle() const {
        return static_cast<CapStyle>(Ptr()->GetStartCap());
    }

    CapStyle GetEndCapStyle() const {
        return static_cast<CapStyle>(Ptr()->GetEndCap());
    }

    CapStyle GetDashCapStyle() const {
        return static_cast<CapStyle>(Ptr()->GetDashCap());
    }

    LineJoinStyle GetLineJoinStyle() const {
        return static_cast<LineJoinStyle>(Ptr()->GetLineJoin());
    }

    float GetMiterLimit() const {
        return Ptr()->GetMiterLimit();
    }

    DashStyle GetDashStyle() const {
        return static_cast<DashStyle>(Ptr()->GetDashStyle());
    }

    float GetDashOffset() const {
        return Ptr()->GetDashOffset();
    }

    std::vector<float> GetDashPattern() const {

        auto count = Ptr()->GetDashesCount();

        std::vector<float> dash_pattern(count);
        Ptr()->GetDashes(dash_pattern.data(), count);
        return dash_pattern;
    }
};

}