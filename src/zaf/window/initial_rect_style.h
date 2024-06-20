#pragma once

#include <zaf/object/enum_declaration.h>
#include <zaf/object/enum_support.h>

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

ZAF_ENUM_BEGIN(InitialRectStyle)
ZAF_ENUM_VALUE(CenterInScreen)
ZAF_ENUM_VALUE(CenterInOwner)
ZAF_ENUM_VALUE(Custom)
ZAF_ENUM_END;

ZAF_ENABLE_ENUM_BOXING(InitialRectStyle)

}