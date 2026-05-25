#pragma once

#include <zaf/base/direct2d.h>

namespace zaf::d2d {

enum class LineCap {
    Flat = D2D1_CAP_STYLE_FLAT,
    Square = D2D1_CAP_STYLE_SQUARE,
    Round = D2D1_CAP_STYLE_ROUND,
    Triangle = D2D1_CAP_STYLE_TRIANGLE,
};

}