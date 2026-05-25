#pragma once

#include <zaf/base/direct2d.h>

namespace zaf::d2d {

enum class DashStyle {
    Solid = D2D1_DASH_STYLE_SOLID,
    Dash = D2D1_DASH_STYLE_DASH,
    Dot = D2D1_DASH_STYLE_DOT,
    DashDot = D2D1_DASH_STYLE_DASH_DOT,
    DashDotDot = D2D1_DASH_STYLE_DASH_DOT_DOT,
    Custom = D2D1_DASH_STYLE_CUSTOM,
};

}