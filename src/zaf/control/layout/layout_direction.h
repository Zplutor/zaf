#pragma once

#include <zaf/object/enum_support.h>

namespace zaf {

enum class LayoutDirection {
    LeftToRight,
    TopToBottom,
};

ZAF_ENUM_BEGIN(LayoutDirection)
ZAF_ENUM_CONSTANT(LeftToRight)
ZAF_ENUM_CONSTANT(TopToBottom)
ZAF_ENUM_END;

}