#pragma once

#include <zaf/object/enum_declaration.h>

namespace zaf {

enum class AxisAlignment {
    Near,
    Far,
    Center,
};

ZAF_DECLARE_ENUM(AxisAlignment)
ZAF_ENABLE_ENUM_BOXING(AxisAlignment)

}