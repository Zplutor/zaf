#pragma once

#include <vector>
#include <zaf/base/com_object.h>
#include <zaf/graphic/d2d/dash_style.h>
#include <zaf/graphic/d2d/line_cap.h>
#include <zaf/graphic/d2d/line_join.h>

namespace zaf::d2d {

class StrokeStyle : public COMObject<ID2D1StrokeStyle> {
public:
    using COMObject::COMObject;

    LineCap GetStartCap() const {
        return static_cast<LineCap>(Ptr()->GetStartCap());
    }

    LineCap GetEndCap() const {
        return static_cast<LineCap>(Ptr()->GetEndCap());
    }

    LineCap GetDashCap() const {
        return static_cast<LineCap>(Ptr()->GetDashCap());
    }

    LineJoin GetLineJoin() const {
        return static_cast<LineJoin>(Ptr()->GetLineJoin());
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