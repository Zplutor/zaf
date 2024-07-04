#pragma once

#include <zaf/base/flags_enum.h>
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

ZAF_ENABLE_FLAGS_ENUM(ActivateOption);

ZAF_ENUM_BEGIN(ActivateOption)
ZAF_ENUM_CONSTANT(Normal)
ZAF_ENUM_CONSTANT(NoActivate)
ZAF_ENUM_CONSTANT(DiscardMouseMessage)
ZAF_ENUM_END;

}