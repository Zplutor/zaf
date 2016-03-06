#pragma once

#include <dwrite.h>
#include <cstddef>

namespace zaf {

class TextRange {
public:
	static TextRange FromDWRITETEXTRANGE(const DWRITE_TEXT_RANGE& text_range) {
		return TextRange(text_range.startPosition, text_range.length);
	}

public:
	TextRange() : position(0), length(0) { }
	TextRange(std::size_t position, std::size_t length) : position(position), length(length) { }

	DWRITE_TEXT_RANGE ToDWRITETEXTRANGE() const {

		DWRITE_TEXT_RANGE text_range;
		text_range.startPosition = position;
		text_range.length = length;
		return text_range;
	}

public:
	std::size_t position;
	std::size_t length;
};

}