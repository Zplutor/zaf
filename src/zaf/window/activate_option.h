#pragma once

#include <zaf/base/flag_enum.h>
#include <zaf/object/enum_declaration.h>

namespace zaf {

/**
 Specifies options when activating a window.

 This enumeration allows a bitwise combination of its member values.
 */
enum class ActivateOption {

    /**
     The window is activated as normal.
     */
    None = 0,

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

ZAF_DECLARE_ENUM(ActivateOption)
ZAF_ENABLE_ENUM_BOXING(ActivateOption)
ZAF_ENABLE_FLAG_ENUM(ActivateOption);

}