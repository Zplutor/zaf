#pragma once

#include <zaf/base/direct2d.h>

namespace zaf::d2d {

enum class LineJoin {
    Miter = D2D1_LINE_JOIN_MITER,
    Bevel = D2D1_LINE_JOIN_BEVEL,
    Round = D2D1_LINE_JOIN_ROUND,
    MiterOrBevel = D2D1_LINE_JOIN_MITER_OR_BEVEL,
};

}