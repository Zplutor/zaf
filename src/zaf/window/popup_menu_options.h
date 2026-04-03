#pragma once

#include <zaf/base/flags_enum.h>

/**
@file
    Defines the `zaf::PopupMenuOptions` enum.
*/

namespace zaf {

/**
Options for configuring the behavior of a popup menu.
*/
enum class PopupMenuOptions {

    /**
    The popup menu's left edge aligns to the specified position.
    */
    AlignLeft = 0,

    /**
    The popup menu's top edge aligns to the specified position.
    */
    AlignTop = 0,

    /**
    The popup menu's right edge aligns to the specified position.
    */
    AlignRight = 1 << 0,

    /**
    The popup menu's bottom edge aligns to the specified position.
    */
    AlignBottom = 1 << 1,

    /**
    The popup menu's top-left corner aligns to the specified position.
    */
    AlignTopLeft = AlignTop | AlignLeft,

    /**
    The popup menu's top-right corner aligns to the specified position.
    */
    AlignTopRight = AlignTop | AlignRight,

    /**
    The popup menu's bottom-left corner aligns to the specified position.
    */
    AlignBottomLeft = AlignBottom | AlignLeft,

    /**
    The popup menu's bottom-right corner aligns to the specified position.
    */
    AlignBottomRight = AlignBottom | AlignRight,

    /**
    Same as `AlignTopLeft`.
    */
    Default = AlignTopLeft,
};

ZAF_ENABLE_FLAGS_ENUM(PopupMenuOptions)

}