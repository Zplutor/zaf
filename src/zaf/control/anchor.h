#pragma once

#include <zaf/base/flag_enum.h>

namespace zaf {

/**
 Specifies how a control anchors to the edges of its parent.  

 This enumeration allows bitwise combination of its values.
 */
enum class Anchor {

    None = 0,

    /**
     The control is anchored to the left edge of its parent.
     */
    Left = 1 << 0,

    /**
     The control is anchored to the top edge of its parent.
     */
    Top = 1 << 1,

    /**
     The control is anchored to the right edge of its parent.
     */
    Right = 1 << 2,

    /**
     The control is anchored to the bottom edge of its parent.
     */
    Bottom = 1 << 3,
};

ZAF_ENABLE_FLAG_ENUM(Anchor);

}