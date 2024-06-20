#pragma once

#include <dwrite.h>
#include <optional>
#include <string>
#include <zaf/object/enum_declaration.h>
#include <zaf/object/enum_support.h>

namespace zaf {

/**
 Specifies the alignment of paragraph text along the reading direction axis,
 relative to the leading and trailing edge of the layout box.
 */
enum class TextAlignment {

    /**
     The leading edge of the paragraph text is aligned to the leading edge of the layout box.
     */
    Leading = DWRITE_TEXT_ALIGNMENT_LEADING,

    /**
     The trailing edge of the paragraph text is aligned to the trailing edge of the layout box.
     */
    Tailing = DWRITE_TEXT_ALIGNMENT_TRAILING,

    /**
     The center of the paragraph text is aligned to the center of the layout box.
     */
    Center = DWRITE_TEXT_ALIGNMENT_CENTER,

    /**
     Align text to the leading side, and also justify text to fill the lines.
     */
    Justified = DWRITE_TEXT_ALIGNMENT_JUSTIFIED,

    Left = Leading,
    Right = Tailing,
};

ZAF_ENUM_BEGIN(TextAlignment)
ZAF_ENUM_VALUE(Leading)
ZAF_ENUM_VALUE(Tailing)
ZAF_ENUM_VALUE(Center)
ZAF_ENUM_VALUE(Justified)
ZAF_ENUM_VALUE(Left)
ZAF_ENUM_VALUE(Right)
ZAF_ENUM_END;

ZAF_ENABLE_ENUM_BOXING(TextAlignment)

}