#pragma once

#include <dwrite.h>
#include <zaf/base/assert.h>
#include <zaf/graphic/text/paragraph_alignment.h>
#include <zaf/graphic/text/text_alignment.h>
#include <zaf/graphic/text/word_wrapping.h>

namespace zaf {

/**
 Describes the font and paragraph properties used to format text.

 You should create TextFormat instances via ResourceFactory::CreateTextFormat.
 */
class TextFormat {
public:
    /**
     Construct the instance with specified handle. 

     The instance takes over the lifetime of handle, and would release the handle
     when destroyed.
     */
	explicit TextFormat(IDWriteTextFormat* handle) : handle_(handle) { 
		ZAF_ASSERT(handle_ != nullptr);
	}

	virtual ~TextFormat() { 
		handle_->Release();
	}

    /**
     Get the handle of text format.
     */
	IDWriteTextFormat* GetHandle() const {
		return handle_;
	}

    /**
     Get the alignment option of text relative to the layout box's leading and trailing edge.
     */
	TextAlignment GetTextAlignment() const {
		return static_cast<TextAlignment>(handle_->GetTextAlignment());
	}

    /**
     Set the alignment option of text relative to the layout box's leading and trailing edge.
     */
	void SetTextAlignment(TextAlignment alignment) {
		handle_->SetTextAlignment(static_cast<DWRITE_TEXT_ALIGNMENT>(alignment));
	}

    /**
     Get the alignment option of a paragraph which is relative to the top and bottom edges of a layout box.
     */
	ParagraphAlignment GetParagraphAlignment() const {
		return static_cast<ParagraphAlignment>(handle_->GetParagraphAlignment());
	}

    /**
     Set the alignment option of a paragraph which is relative to the top and bottom edges of a layout box.
     */
	void SetParagraphAlignment(ParagraphAlignment alignment) {
		handle_->SetParagraphAlignment(static_cast<DWRITE_PARAGRAPH_ALIGNMENT>(alignment));
	}

    /**
     Get the word wrapping option.
     */
	WordWrapping GetWordWrapping() const {
		return static_cast<WordWrapping>(handle_->GetWordWrapping());
	}

    /**
     Set the word wrapping option.
     */
	void SetWordWrapping(WordWrapping word_wrapping) {
		handle_->SetWordWrapping(static_cast<DWRITE_WORD_WRAPPING>(word_wrapping));
	}

private:
	IDWriteTextFormat* handle_;
};

}