#pragma once

#include <dwrite.h>
#include <cstdint>

namespace zaf::dwrite {

/**
 Contains information about a formatted line of text.
 */
class LineMetrics : private DWRITE_LINE_METRICS {
public:
    /**
     The number of text positions in the text line. This includes any trailing whitespace
     and newline characters.
     */
    std::uint32_t Length() const {
        return length;
    }

    /**
     The number of whitespace positions at the end of the text line. Newline sequences are 
     considered whitespace.
     */
    std::uint32_t TrailingWhitespaceLength() const {
        return trailingWhitespaceLength;
    }

    /**
     The number of characters in the newline sequence at the end of the text line. If the
     count is zero, then the text line was either wrapped or it is the end of the text.
     */
    std::uint32_t NewlineLength() const {
        return newlineLength;
    }

    /**
     The height of the text line.
     */
    float Height() const {
        return height;
    }

    /**
     The distance from the top of the text line to its baseline.
     */
    float Baseline() const {
        return baseline;
    }

    /**
     The line is trimmed or not.
     */
    bool IsTrimmed() const {
        return isTrimmed == TRUE;
    }
};


static_assert(sizeof(LineMetrics) == sizeof(DWRITE_LINE_METRICS));

}