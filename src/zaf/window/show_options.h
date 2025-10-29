#pragma once

/**
@file
    Defines the `zaf::ShowOptions` enum.
*/

#include <zaf/base/flags_enum.h>

namespace zaf {

/**
Specifies options when showing a window.
*/
enum class ShowOptions {

    /**
    The window is shown normally.

    @details
        The window will be activated when it is shown, except that it has the
        `zaf::ActivateOptions::NoAutoActivate` option.
    */
    Normal = 0,

    /**
    The window is shown but not activated.

    @details
        With this option, the window will be shown without being activated.
    */
    NoActivate = 1 << 0,
};

ZAF_ENABLE_FLAGS_ENUM(ShowOptions);

}