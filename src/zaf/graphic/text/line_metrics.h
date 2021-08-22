#pragma once

#include <dwrite.h>
#include <cstdint>

namespace zaf {

/**
 Contains information about a formatted line of text.
 */
class LineMetrics {
public:
    LineMetrics() : 
        length(0),
        trailing_whitespace_length(0),
        newline_length(0),
        height(0),
        baseline(0),
        is_trimmed(false) {
    
    }

    /**
     The number of text positions in the text line. This includes any trailing whitespace
     and newline characters.
     */
    std::uint32_t length;

    /**
     The number of whitespace positions at the end of the text line. Newline sequences are 
     considered whitespace.
     */
    std::uint32_t trailing_whitespace_length;

    /**
     The number of characters in the newline sequence at the end of the text line. If the
     count is zero, then the text line was either wrapped or it is the end of the text.
     */
    std::uint32_t newline_length;

    /**
     The height of the text line.
     */
    float height;

    /**
     The distance from the top of the text line to its baseline.
     */
    float baseline;

    /**
     The line is trimmed or not.
     */
    bool is_trimmed;
};

}