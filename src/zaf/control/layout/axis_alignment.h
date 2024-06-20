#pragma once

#include <zaf/object/enum_declaration.h>
#include <zaf/object/enum_support.h>

namespace zaf {

enum class AxisAlignment {
    Start,
    End,
    Center,
};

ZAF_ENUM_BEGIN(AxisAlignment)
ZAF_ENUM_VALUE(Start)
ZAF_ENUM_VALUE(End)
ZAF_ENUM_VALUE(Center)
ZAF_ENUM_END;

ZAF_ENABLE_ENUM_BOXING(AxisAlignment)

}