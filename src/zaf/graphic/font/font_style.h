#pragma once

#include <dwrite.h>
#include <zaf/object/enum_declaration.h>

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

ZAF_DECLARE_ENUM(FontStyle)
ZAF_ENABLE_ENUM_BOXING(FontStyle)

}