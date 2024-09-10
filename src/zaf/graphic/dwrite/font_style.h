#pragma once

#include <dwrite.h>
#include <zaf/object/enum_support.h>

namespace zaf {

/**
 Defines styles of a font.   
 */
enum class FontStyle {

    /**
     The font is normal.
     */
    Normal = DWRITE_FONT_STYLE_NORMAL,

    /**
     The font is italic.
     */
    Italic = DWRITE_FONT_STYLE_ITALIC,

    /**
     The font is oblique.
     */
    Oblique = DWRITE_FONT_STYLE_OBLIQUE,
};

ZAF_ENUM_BEGIN(FontStyle)
ZAF_ENUM_CONSTANT(Normal)
ZAF_ENUM_CONSTANT(Italic)
ZAF_ENUM_CONSTANT(Oblique)
ZAF_ENUM_END;

}