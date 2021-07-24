#pragma once

#include <zaf/object/enum_declaration.h>

namespace zaf {

/**
 The initial rect style of a window.
 */
enum class InitialRectStyle {

    /**
     The window is centered whithin the screen, and has a specified size set by
     the SetRect method.
     */
    CenterInScreen,

    /**
     The window is centered within its owner window, and has a specified size
     set by the SetRect method.
     */
     CenterInOwner,

     /**
      The window's rect is specifed by the SetRect method.
      */
      Custom,
};

ZAF_DECLARE_ENUM(InitialRectStyle)
ZAF_ENABLE_ENUM_BOXING(InitialRectStyle)

}