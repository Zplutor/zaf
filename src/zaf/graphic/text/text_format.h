#pragma once

#include <dwrite.h>
#include <zaf/base/com_ptr.h>
#include <zaf/graphic/text/paragraph_alignment.h>
#include <zaf/graphic/text/text_alignment.h>
#include <zaf/graphic/text/text_trimming.h>
#include <zaf/graphic/text/word_wrapping.h>

namespace zaf {

/**
 Describes the font and paragraph properties used to format text.

 You should create TextFormat instances via GraphicFactory::CreateTextFormat.
 */
class TextFormat : public COMPtr<IDWriteTextFormat> {
public:
    TextFormat() { }

    /**
     Construct the instance with specified handle. 

     The instance takes over the lifetime of handle, and would release the handle
     when destroyed.
     */
    explicit TextFormat(IDWriteTextFormat* handle) : COMPtr(handle) { 
        
    }

    /**
     Get the alignment option of text relative to the layout box's leading and trailing edge.
     */
    TextAlignment GetTextAlignment() const {
        return static_cast<TextAlignment>(Inner()->GetTextAlignment());
    }

    /**
     Set the alignment option of text relative to the layout box's leading and trailing edge.
     */
    void SetTextAlignment(TextAlignment alignment) {
        Inner()->SetTextAlignment(static_cast<DWRITE_TEXT_ALIGNMENT>(alignment));
    }

    /**
     Get the alignment option of a paragraph which is relative to the top and bottom edges of a layout box.
     */
    ParagraphAlignment GetParagraphAlignment() const {
        return static_cast<ParagraphAlignment>(Inner()->GetParagraphAlignment());
    }

    /**
     Set the alignment option of a paragraph which is relative to the top and bottom edges of a layout box.
     */
    void SetParagraphAlignment(ParagraphAlignment alignment) {
        Inner()->SetParagraphAlignment(static_cast<DWRITE_PARAGRAPH_ALIGNMENT>(alignment));
    }

    /**
     Get the word wrapping option.
     */
    WordWrapping GetWordWrapping() const {
        return static_cast<WordWrapping>(Inner()->GetWordWrapping());
    }

    /**
     Set the word wrapping option.
     */
    void SetWordWrapping(WordWrapping word_wrapping) {
        Inner()->SetWordWrapping(static_cast<DWRITE_WORD_WRAPPING>(word_wrapping));
    }

    TextTrimming GetTextTrimming() const;
    void SetTextTrimming(const TextTrimming& trimming);
};

}