#pragma once

#include <vector>
#include <zaf/base/direct2d.h>

namespace zaf {

class Stroke {
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

    class Properties {
    public:
        Properties() : 
            start_cap_style(CapStyle::Flat),
            end_cap_style(CapStyle::Flat),
            dash_cap_style(CapStyle::Flat),
            line_join_style(LineJoinStyle::Miter),
            miter_limit(0),
            dash_style(DashStyle::Solid),
            dash_offset(0) { }

        CapStyle start_cap_style;
        CapStyle end_cap_style;
        CapStyle dash_cap_style;
        LineJoinStyle line_join_style;
        float miter_limit;
        DashStyle dash_style;
        float dash_offset;
        std::vector<float> dash_pattern;
    };

public:
    explicit Stroke(ID2D1StrokeStyle* handle) : handle_(handle) { }

    ~Stroke() {
        if (handle_ != nullptr) {
            handle_->Release();
        }
    }

    CapStyle GetStartCapStyle() const {
        return static_cast<CapStyle>(handle_->GetStartCap());
    }

    CapStyle GetEndCapStyle() const {
        return static_cast<CapStyle>(handle_->GetEndCap());
    }

    CapStyle GetDashCapStyle() const {
        return static_cast<CapStyle>(handle_->GetDashCap());
    }

    LineJoinStyle GetLineJoinStyle() const {
        return static_cast<LineJoinStyle>(handle_->GetLineJoin());
    }

    float GetMiterLimit() const {
        return handle_->GetMiterLimit();
    }

    DashStyle GetDashStyle() const {
        return static_cast<DashStyle>(handle_->GetDashStyle());
    }

    float GetDashOffset() const {
        return handle_->GetDashOffset();
    }

    std::vector<float> GetDashPattern() const {

        std::size_t count = handle_->GetDashesCount();

        std::vector<float> dash_pattern(count);
        handle_->GetDashes(dash_pattern.data(), count);
        return dash_pattern;
    }

	ID2D1StrokeStyle* GetHandle() const {
		return handle_;
	}

private:
    ID2D1StrokeStyle* handle_;
};

}