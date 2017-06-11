#pragma once

#include <dwrite.h>
#include <string>
#include <zaf/base/optional.h>
#include <zaf/serialization/serializer.h>

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
};


std::wstring ConvertParagraphAlignmentToString(ParagraphAlignment paragraph_alignment);
optional<ParagraphAlignment> ConvertParagraphAlignmentFromString(const std::wstring& string);


template<>
class Serializer<ParagraphAlignment> {
public:
    static std::shared_ptr<DataNode> Serialize(ParagraphAlignment paragraph_alignment) {
        return DataNode::CreateString(ConvertParagraphAlignmentToString(paragraph_alignment));
    }
};

}