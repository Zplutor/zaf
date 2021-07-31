#pragma once

#include <zaf/object/enum_declaration.h>

namespace zaf {

enum class LayoutDirection {
    LeftToRight,
    TopToBottom,
};

ZAF_DECLARE_ENUM(LayoutDirection)
ZAF_ENABLE_ENUM_BOXING(LayoutDirection)

}