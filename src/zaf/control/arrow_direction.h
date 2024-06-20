#pragma once

#include <zaf/object/enum_declaration.h>
#include <zaf/object/enum_support.h>

namespace zaf {

/**
 Defines directions of an arrow.
*/
enum class ArrowDirection {

    /**
    The arrow's direction is left.
    */
    Left,

    /**
    The arrow's direction is up.
    */
    Up,

    /**
    The arrow's direction is right.
    */
    Right,

    /**
    The arrow's direction is down.
    */
    Down,
};

ZAF_ENUM_BEGIN(ArrowDirection)
ZAF_ENUM_VALUE(Left)
ZAF_ENUM_VALUE(Up)
ZAF_ENUM_VALUE(Right)
ZAF_ENUM_VALUE(Down)
ZAF_ENUM_END;

ZAF_ENABLE_ENUM_BOXING(ArrowDirection)

}