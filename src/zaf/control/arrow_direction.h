#pragma once

#include <zaf/object/enum_declaration.h>

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

ZAF_DECLARE_ENUM(ArrowDirection)
ZAF_ENABLE_ENUM_BOXING(ArrowDirection)

}