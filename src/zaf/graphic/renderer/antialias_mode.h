#pragma once

#include <zaf/base/direct2d.h>

namespace zaf {

enum class AntialiasMode {
    PerPrimitive = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
    Aliased = D2D1_ANTIALIAS_MODE_ALIASED,
};

}