#pragma once

#include <zaf/object/enum_support.h>

namespace zaf {

enum class AxisAlignment {
    Start,
    End,
    Center,
};

ZAF_ENUM_BEGIN(AxisAlignment)
ZAF_ENUM_CONSTANT(Start)
ZAF_ENUM_CONSTANT(End)
ZAF_ENUM_CONSTANT(Center)
ZAF_ENUM_END;

}