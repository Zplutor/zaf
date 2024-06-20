#pragma once

#include <zaf/base/flag_enum.h>
#include <zaf/object/enum_declaration.h>
#include <zaf/object/enum_support.h>

namespace zaf {

/**
 Specifies options when activating a window.

 This enumeration allows a bitwise combination of its member values.
 */
enum class ActivateOption {

    /**
     The window is activated as normal.
     */
    Normal = 0,

    /**
     The window is not allow to be activated.

     A window with this value would not be shown in the taskbar.
     */
    NoActivate = 1,

    /**
     The mouse message that triggers the activating would be discard.
     */
    DiscardMouseMessage = 2,
};

ZAF_ENUM_BEGIN(ActivateOption)
ZAF_ENUM_VALUE(Normal)
ZAF_ENUM_VALUE(NoActivate)
ZAF_ENUM_VALUE(DiscardMouseMessage)
ZAF_ENUM_END;

ZAF_ENABLE_ENUM_BOXING(ActivateOption)
ZAF_ENABLE_FLAG_ENUM(ActivateOption);

}