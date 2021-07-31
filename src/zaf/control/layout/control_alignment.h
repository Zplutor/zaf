#pragma once

#include <zaf/object/enum_declaration.h>

namespace zaf {

enum class ControlAlignment {
    Leading,
    Tailing,
    Center,
};

ZAF_DECLARE_ENUM(ControlAlignment)
ZAF_ENABLE_ENUM_BOXING(ControlAlignment)

}