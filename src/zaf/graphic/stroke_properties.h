#pragma once

#include <zaf/graphic/stroke.h>

namespace zaf {

class StrokeProperties {
public:
    StrokeProperties() :
        start_cap_style(Stroke::CapStyle::Flat),
        end_cap_style(Stroke::CapStyle::Flat),
        dash_cap_style(Stroke::CapStyle::Flat),
        line_join_style(Stroke::LineJoinStyle::Miter),
        miter_limit(0),
        dash_style(Stroke::DashStyle::Solid),
        dash_offset(0) { }

    Stroke::CapStyle start_cap_style;
    Stroke::CapStyle end_cap_style;
    Stroke::CapStyle dash_cap_style;
    Stroke::LineJoinStyle line_join_style;
    float miter_limit;
    Stroke::DashStyle dash_style;
    float dash_offset;
    std::vector<float> dash_pattern;
};

}