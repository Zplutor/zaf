#include <zaf/graphic/text/text_layout.h>
#include <memory>

namespace zaf {

std::wstring TextLayout::GetFontFamilyName(std::size_t position, TextRange* range, std::error_code& error_code) const {

	std::size_t family_name_length = 0;
	HRESULT result = GetHandle()->GetFontFamilyNameLength(position, &family_name_length);

    error_code = MakeComErrorCode(result);
    if (! IsSucceeded(error_code)) {
        return std::wstring();
    }

	if (family_name_length == 0) {
		return std::wstring();
	}

    const std::size_t buffer_length = family_name_length + 1;
    auto buffer = std::make_unique<wchar_t[]>(buffer_length);
	DWRITE_TEXT_RANGE text_range = { 0 };
	result = GetHandle()->GetFontFamilyName(
        position,
        buffer.get(), 
        buffer_length, 
        range == nullptr ? nullptr : &text_range);

    error_code = MakeComErrorCode(result);
    if (! IsSucceeded(error_code)) {
        return std::wstring();
    }

	if (range != nullptr) {
		*range = TextRange::FromDWRITETEXTRANGE(text_range);
	}

	return buffer.get();
}


float TextLayout::GetFontSize(std::size_t position, TextRange* range, std::error_code& error_code) const {

	float font_size = 0;
	DWRITE_TEXT_RANGE text_range = { 0 };
	HRESULT result = GetHandle()->GetFontSize(position, &font_size, range == nullptr ? nullptr : &text_range);

    error_code = MakeComErrorCode(result);
    if (! IsSucceeded(error_code)) {
        return 0;
    }
	
	if (range != nullptr) {
		*range = TextRange::FromDWRITETEXTRANGE(text_range);
	}

	return font_size;
}


FontStyle TextLayout::GetFontStyle(std::size_t position, TextRange* range, std::error_code& error_code) const {

	DWRITE_FONT_STYLE font_style = DWRITE_FONT_STYLE_NORMAL;
	DWRITE_TEXT_RANGE text_range = { 0 };
	HRESULT result = GetHandle()->GetFontStyle(position, &font_style, range == nullptr ? nullptr : &text_range);

    error_code = MakeComErrorCode(result);
    if (! IsSucceeded(error_code)) {
        return FontStyle::Normal;
    }

	if (range != nullptr) {
		*range = TextRange::FromDWRITETEXTRANGE(text_range);
	}

	return static_cast<FontStyle>(font_style);
}


int TextLayout::GetFontWeight(std::size_t position, TextRange* range, std::error_code& error_code) const {

	DWRITE_FONT_WEIGHT font_weight = static_cast<DWRITE_FONT_WEIGHT>(0);
	DWRITE_TEXT_RANGE text_range = { 0 };
	HRESULT result = GetHandle()->GetFontWeight(position, &font_weight, range == nullptr ? nullptr : &text_range);

    error_code = MakeComErrorCode(result);
    if (! IsSucceeded(error_code)) {
        return 0;
    }

	if (range != nullptr) {
		*range = TextRange::FromDWRITETEXTRANGE(text_range);
	}

	return font_weight;
}


bool TextLayout::HasUnderline(std::size_t position, TextRange* range, std::error_code& error_code) const {

	BOOL has_underline = FALSE;
	DWRITE_TEXT_RANGE text_range = { 0 };
	HRESULT result = GetHandle()->GetUnderline(position, &has_underline, range == nullptr ? nullptr : &text_range);

    error_code = MakeComErrorCode(result);
    if (!IsSucceeded(error_code)) {
        return false;
    }

	if (range != nullptr) {
		*range = TextRange::FromDWRITETEXTRANGE(text_range);
	}

	return has_underline ? TRUE : FALSE;
}


const std::vector<LineMetrics> TextLayout::GetLineMetrics(std::size_t max_line_count, std::error_code& error_code) const {

	auto dwrite_line_metrics = std::make_unique<DWRITE_LINE_METRICS[]>(max_line_count);
	std::size_t actual_line_count = 0;
	HRESULT result = GetHandle()->GetLineMetrics(dwrite_line_metrics.get(), max_line_count, &actual_line_count);

    error_code = MakeComErrorCode(result);
	if (! IsSucceeded(error_code)) {
		return std::vector<LineMetrics>();
	}

	std::vector<LineMetrics> line_metrics;
	line_metrics.reserve(actual_line_count);

	for (std::size_t index = 0; index < actual_line_count; ++index) {

		const DWRITE_LINE_METRICS& each_dwrite_line_metrics = dwrite_line_metrics.get()[index];

		LineMetrics each_line_metrics;
		each_line_metrics.length = each_dwrite_line_metrics.length;
		each_line_metrics.trailing_whitespace_length = each_dwrite_line_metrics.trailingWhitespaceLength;
		each_line_metrics.newline_length = each_dwrite_line_metrics.newlineLength;
		each_line_metrics.height = each_dwrite_line_metrics.height;
		each_line_metrics.baseline = each_dwrite_line_metrics.baseline;
		each_line_metrics.is_trimmed = each_dwrite_line_metrics.isTrimmed != FALSE;

		line_metrics.push_back(each_line_metrics);
	}

	return line_metrics;
}


const TextMetrics TextLayout::GetMetrics(std::error_code& error_code) const {

	DWRITE_TEXT_METRICS dwrite_text_metrics = { 0 };
    HRESULT result = GetHandle()->GetMetrics(&dwrite_text_metrics);

    error_code = MakeComErrorCode(result);
    if (! IsSucceeded(error_code)) {
        return TextMetrics();
    }

	TextMetrics text_metrics;
	text_metrics.left = dwrite_text_metrics.left;
	text_metrics.top = dwrite_text_metrics.top;
	text_metrics.width = dwrite_text_metrics.width;
	text_metrics.height = dwrite_text_metrics.height;
	text_metrics.width_including_trailing_whitespace = dwrite_text_metrics.widthIncludingTrailingWhitespace;
	text_metrics.max_bidi_reordering_depth = dwrite_text_metrics.maxBidiReorderingDepth;
	return text_metrics;
}

}