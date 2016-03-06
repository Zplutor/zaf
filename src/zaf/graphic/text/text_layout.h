#pragma once

#include <cstddef>
#include <string>
#include <zaf/graphic/font/font_style.h>
#include <zaf/graphic/text/text_format.h>
#include <zaf/graphic/text/text_range.h>

namespace zaf {

class TextRange;

class TextLayout : public TextFormat {
public:
	TextLayout(IDWriteTextLayout* handle) : 
		TextFormat(handle), 
		handle_(handle) { 
	
		ZAF_ASSERT(handle_ != nullptr);
	}

	std::wstring GetFontFamilyName(std::size_t position) const {
		return GetFontFamilyName(position, nullptr);
	}

	std::wstring GetFontFamilyName(std::size_t position, TextRange* range) const;

	float GetFontSize(std::size_t position) const {
		return GetFontSize(position, nullptr);
	}

	void SetFontFamilyName(const TextRange& range, const std::wstring& font_family_name) {
		handle_->SetFontFamilyName(font_family_name.c_str(), range.ToDWRITETEXTRANGE());
	}

	float GetFontSize(std::size_t position, TextRange* range) const;

	void SetFontSize(const TextRange& range, float size) {
		handle_->SetFontSize(size, range.ToDWRITETEXTRANGE());
	}

	FontStyle GetFontStyle(std::size_t position) const {
		return GetFontStyle(position, nullptr);
	}

	FontStyle GetFontStyle(std::size_t position, TextRange* range) const;

	void SetFontStyle(const TextRange& range, FontStyle font_style) {
		handle_->SetFontStyle(static_cast<DWRITE_FONT_STYLE>(font_style), range.ToDWRITETEXTRANGE());
	}

	int GetFontWeight(std::size_t position) const {
		return GetFontWeight(position, nullptr);
	}

	int GetFontWeight(std::size_t position, TextRange* range) const;

	void SetFontWeight(const TextRange& range, int weight) {
		handle_->SetFontWeight(static_cast<DWRITE_FONT_WEIGHT>(weight), range.ToDWRITETEXTRANGE());
	}

	bool HasUnderline(std::size_t position) const {
		return HasUnderline(position, nullptr);
	}

	bool HasUnderline(std::size_t position, TextRange* range) const;

	void SetHasUnderline(const TextRange& range, bool has_underline) {
		handle_->SetUnderline(has_underline ? TRUE : FALSE, range.ToDWRITETEXTRANGE());
	}

private:
	IDWriteTextLayout* handle_;
};

}