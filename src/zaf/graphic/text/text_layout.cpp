#include <zaf/graphic/text/text_layout.h>

namespace zaf {

std::wstring TextLayout::GetFontFamilyName(std::size_t position, TextRange* range) const {

	std::size_t family_name_length = 0;
	handle_->GetFontFamilyNameLength(position, &family_name_length);

	if (family_name_length == 0) {
		return std::wstring();
	}

	std::wstring family_name(family_name_length + 1, 0);
	DWRITE_TEXT_RANGE text_range = { 0 };
	handle_->GetFontFamilyName(position, &family_name[0], family_name.length(), range == nullptr ? nullptr : &text_range);

	if (range != nullptr) {
		*range = TextRange::FromDWRITETEXTRANGE(text_range);
	}

	family_name.resize(family_name_length);
	return family_name;
}


float TextLayout::GetFontSize(std::size_t position, TextRange* range) const {

	float font_size = 0;
	DWRITE_TEXT_RANGE text_range = { 0 };
	handle_->GetFontSize(position, &font_size, range == nullptr ? nullptr : &text_range);
	
	if (range != nullptr) {
		*range = TextRange::FromDWRITETEXTRANGE(text_range);
	}

	return font_size;
}


FontStyle TextLayout::GetFontStyle(std::size_t position, TextRange* range) const {

	DWRITE_FONT_STYLE font_style = DWRITE_FONT_STYLE_NORMAL;
	DWRITE_TEXT_RANGE text_range = { 0 };
	handle_->GetFontStyle(position, &font_style, range == nullptr ? nullptr : &text_range);

	if (range != nullptr) {
		*range = TextRange::FromDWRITETEXTRANGE(text_range);
	}

	return static_cast<FontStyle>(font_style);
}


int TextLayout::GetFontWeight(std::size_t position, TextRange* range) const {

	DWRITE_FONT_WEIGHT font_weight = static_cast<DWRITE_FONT_WEIGHT>(0);
	DWRITE_TEXT_RANGE text_range = { 0 };
	handle_->GetFontWeight(position, &font_weight, range == nullptr ? nullptr : &text_range);

	if (range != nullptr) {
		*range = TextRange::FromDWRITETEXTRANGE(text_range);
	}

	return font_weight;
}


bool TextLayout::HasUnderline(std::size_t position, TextRange* range) const {

	BOOL has_underline = FALSE;
	DWRITE_TEXT_RANGE text_range = { 0 };
	handle_->GetUnderline(position, &has_underline, range == nullptr ? nullptr : &text_range);

	if (range != nullptr) {
		*range = TextRange::FromDWRITETEXTRANGE(text_range);
	}

	return has_underline ? TRUE : FALSE;
}

}