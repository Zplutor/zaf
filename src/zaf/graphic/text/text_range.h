#pragma once

#include <dwrite.h>

namespace zaf {

/**
 Describes a pair of start index and length in a text.   
 */
class TextRange {
public:
    /**
     Convert a DWRITE_TEXT_RANGE structure to TextRange.
     */
	static TextRange FromDWRITETEXTRANGE(const DWRITE_TEXT_RANGE& text_range) {
		return TextRange(text_range.startPosition, text_range.length);
	}

public:
    /**
     Construct the instance with zero index and zero length.
     */
	TextRange() : index(0), length(0) { }

    /**
     Construct the instance with specified index and length.
     */
	TextRange(std::size_t index, std::size_t length) : index(index), length(length) { }

    /**
     Convert the instance to a DWRITE_TEXT_RANGE structure.
     */
	DWRITE_TEXT_RANGE ToDWRITETEXTRANGE() const {

		DWRITE_TEXT_RANGE text_range;
		text_range.startPosition = index;
		text_range.length = length;
		return text_range;
	}

public:
    /**
     The start index.
     */
	std::size_t index;

    /**
     The length.
     */
	std::size_t length;
};

}