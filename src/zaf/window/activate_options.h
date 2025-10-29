#pragma once

/**
@file
    Defines the `zaf::ActivateOptions` enum.
*/

#include <zaf/base/flags_enum.h>
#include <zaf/object/enum_support.h>

namespace zaf {

/**
Specifies options about activating a window.

@details
    This enumeration allows a bitwise combination of its member values.
*/
enum class ActivateOptions {

    /**
    The window can be activated normally.
    */
    Normal = 0,

    /**
    The window cannot be activated automatically.

    @details
        With this option, the window will be applied the WS_EX_NOACTIVATE extended style and won't
        be activated when:
        - The user clicks on the window while the currently activated window belongs to another 
          thread. It can still be activated if the latter belongs to the same thread.
        - The window is beneath the currently activated window and the latter is closed or 
          minimized.
        - Calling `Show()` to show the window.

        A window with this option would not be shown in the taskbar as well.
    */
    NoAutoActivate = 1 << 0,

    /**
    The window cannot be activated by mouse click.

    @details
        With this option, the window will handle the WM_MOUSEACTIVATE message and return 
        MA_NOACTIVATE or MA_NOACTIVATEANDEAT, depends on whether the `DiscardMouseMessage` option 
        is also set. This can be used to prevent the window from being activated by mouse click
        even if the currently activated window belongs to the same thread.
    */
    NoMouseActivate = 1 << 1,

    /**
    A shorthand combination of `NoAutoActivate` and `NoMouseActivate`.
    */
    NoActivate = NoAutoActivate | NoMouseActivate,

    /**
    The mouse message that triggers the activation will be discarded.

    @details
        With this option, the window will handle the WM_MOUSEACTIVATE message and return
        MA_ACTIVATEANDEAT or MA_NOACTIVATEANDEAT, depends on whether the `NoMouseActivate` option
        is also set.
    */
    DiscardMouseMessage = 1 << 2,
};

ZAF_ENABLE_FLAGS_ENUM(ActivateOptions);

ZAF_ENUM_BEGIN(ActivateOptions)
ZAF_ENUM_CONSTANT(Normal)
ZAF_ENUM_CONSTANT(NoAutoActivate)
ZAF_ENUM_CONSTANT(NoMouseActivate)
ZAF_ENUM_CONSTANT(NoActivate)
ZAF_ENUM_CONSTANT(DiscardMouseMessage)
ZAF_ENUM_END;

}