#pragma once

#include <dwrite.h>
#include <optional>
#include <string>
#include <zaf/object/enum_declaration.h>
#include <zaf/object/enum_support.h>

namespace zaf {

/**
 Specifies the alignment of paragraph text along the flow direction axis,
 relative to the top and bottom of the flow's layout box.
 */
enum class ParagraphAlignment {

    /**
     The top of the text flow is aligned to the top edge of the layout box.
     */
    Near = DWRITE_PARAGRAPH_ALIGNMENT_NEAR,

    /**
     The bottom of the text flow is aligned to the bottom edge of the layout box.
     */
    Far = DWRITE_PARAGRAPH_ALIGNMENT_FAR,

    /**
     The center of the flow is aligned to the center of the layout box.
     */
    Center = DWRITE_PARAGRAPH_ALIGNMENT_CENTER,

    Top = Near,
    Bottom = Far,
};

ZAF_ENUM_BEGIN(ParagraphAlignment)
ZAF_ENUM_VALUE(Near)
ZAF_ENUM_VALUE(Far)
ZAF_ENUM_VALUE(Center)
ZAF_ENUM_VALUE(Top)
ZAF_ENUM_VALUE(Bottom)
ZAF_ENUM_END;

ZAF_ENABLE_ENUM_BOXING(ParagraphAlignment)

}