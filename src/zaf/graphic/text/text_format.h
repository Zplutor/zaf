#pragma once

#include <dwrite.h>
#include <zaf/graphic/text/paragraph_alignment.h>
#include <zaf/graphic/text/text_alignment.h>
#include <zaf/graphic/text/word_wrapping.h>

namespace zaf {

class FontCollection;

class TextFormat {
public:
	class Properties {
	public:
		Properties() : font_size(0), font_weight(0) { }

		std::shared_ptr<FontCollection> font_collection;
		std::wstring font_family_name;
		float font_size;
		int font_weight;
	};

public:
	explicit TextFormat(IDWriteTextFormat* handle) : handle_(handle) { }

	~TextFormat() { 
		handle_->Release();
	}

	IDWriteTextFormat* GetHandle() const {
		return handle_;
	}

	TextAlignment GetTextAlignment() const {
		return static_cast<TextAlignment>(handle_->GetTextAlignment());
	}

	void SetTextAlignment(TextAlignment alignment) {
		handle_->SetTextAlignment(static_cast<DWRITE_TEXT_ALIGNMENT>(alignment));
	}

	ParagraphAlignment GetParagraphAlignment() const {
		return static_cast<ParagraphAlignment>(handle_->GetParagraphAlignment());
	}

	void SetParagraphAlignment(ParagraphAlignment alignment) {
		handle_->SetParagraphAlignment(static_cast<DWRITE_PARAGRAPH_ALIGNMENT>(alignment));
	}

	WordWrapping GetWordWrapping() const {
		return static_cast<WordWrapping>(handle_->GetWordWrapping());
	}

	void SetWordWrapping(WordWrapping word_wrapping) {
		handle_->SetWordWrapping(static_cast<DWRITE_WORD_WRAPPING>(word_wrapping));
	}

private:
	IDWriteTextFormat* handle_;
};

}