#pragma once

#include <dwrite.h>
#include <Richedit.h>

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

    /**
    Convers a CHARRANGE structure to TextRange.
    */
    static TextRange FromCHARRANGE(const CHARRANGE& char_range) {

        auto length = char_range.cpMax - char_range.cpMin;

        return TextRange{
            static_cast<std::size_t>(char_range.cpMin < 0 ? 0 : char_range.cpMin),
            static_cast<std::size_t>(
                length < 0 ? (std::numeric_limits<std::size_t>::max)() : length)
        };
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

    bool Contain(std::size_t position) const {
        return (index <= position) && (position < index + length);
    }

    /**
     Convert the instance to a DWRITE_TEXT_RANGE structure.
     */
    DWRITE_TEXT_RANGE ToDWRITETEXTRANGE() const {

        DWRITE_TEXT_RANGE text_range;
        text_range.startPosition = static_cast<UINT32>(index);
        text_range.length = static_cast<UINT32>(length);
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