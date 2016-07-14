#pragma once

#include <dwrite.h>
#include <zaf/base/define.h>

namespace zaf {

class TextRange {
public:
	static TextRange FromDWRITETEXTRANGE(const DWRITE_TEXT_RANGE& text_range) {
		return TextRange(text_range.startPosition, text_range.length);
	}

public:
	TextRange() : index(InvalidIndex), length(0) { }
	TextRange(std::size_t index, std::size_t length) : index(index), length(length) { }

	DWRITE_TEXT_RANGE ToDWRITETEXTRANGE() const {

		DWRITE_TEXT_RANGE text_range;
		text_range.startPosition = index;
		text_range.length = length;
		return text_range;
	}

public:
	std::size_t index;
	std::size_t length;
};

}