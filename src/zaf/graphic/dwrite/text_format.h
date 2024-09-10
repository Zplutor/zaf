#pragma once

#include <dwrite.h>
#include <zaf/base/com_object.h>
#include <zaf/graphic/dwrite/line_spacing.h>
#include <zaf/graphic/dwrite/paragraph_alignment.h>
#include <zaf/graphic/dwrite/text_alignment.h>
#include <zaf/graphic/dwrite/text_trimming.h>
#include <zaf/graphic/dwrite/word_wrapping.h>

namespace zaf::dwrite {

/**
 Describes the font and paragraph properties used to format text.

 You should create TextFormat instances via GraphicFactory::CreateTextFormat.
 */
class TextFormat : public COMObject<IDWriteTextFormat> {
public:
    using COMObject::COMObject;

    virtual ~TextFormat() = default;

    /**
     Get the alignment option of text relative to the layout box's leading and trailing edge.
     */
    TextAlignment GetTextAlignment() const {
        return static_cast<TextAlignment>(Ptr()->GetTextAlignment());
    }

    /**
     Set the alignment option of text relative to the layout box's leading and trailing edge.
     */
    void SetTextAlignment(TextAlignment alignment) {
        Ptr()->SetTextAlignment(static_cast<DWRITE_TEXT_ALIGNMENT>(alignment));
    }

    /**
     Get the alignment option of a paragraph which is relative to the top and bottom edges of a layout box.
     */
    ParagraphAlignment GetParagraphAlignment() const {
        return static_cast<ParagraphAlignment>(Ptr()->GetParagraphAlignment());
    }

    /**
     Set the alignment option of a paragraph which is relative to the top and bottom edges of a layout box.
     */
    void SetParagraphAlignment(ParagraphAlignment alignment) {
        Ptr()->SetParagraphAlignment(static_cast<DWRITE_PARAGRAPH_ALIGNMENT>(alignment));
    }

    /**
     Get the word wrapping option.
     */
    WordWrapping GetWordWrapping() const {
        return static_cast<WordWrapping>(Ptr()->GetWordWrapping());
    }

    /**
     Set the word wrapping option.
     */
    void SetWordWrapping(WordWrapping word_wrapping) {
        Ptr()->SetWordWrapping(static_cast<DWRITE_WORD_WRAPPING>(word_wrapping));
    }

    TextTrimming GetTextTrimming() const;
    void SetTextTrimming(const TextTrimming& trimming);

    LineSpacing GetLineSpacing() const;
    void SetLineSpacing(const LineSpacing& line_spacing);
};

}