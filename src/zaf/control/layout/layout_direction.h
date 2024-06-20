#pragma once

#include <zaf/object/enum_declaration.h>
#include <zaf/object/enum_support.h>

namespace zaf {

enum class LayoutDirection {
    LeftToRight,
    TopToBottom,
};

ZAF_ENUM_BEGIN(LayoutDirection)
ZAF_ENUM_VALUE(LeftToRight)
ZAF_ENUM_VALUE(TopToBottom)
ZAF_ENUM_END;

ZAF_ENABLE_ENUM_BOXING(LayoutDirection)

}