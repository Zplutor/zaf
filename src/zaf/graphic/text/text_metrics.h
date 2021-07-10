#pragma once

#include <cstdint>

namespace zaf {

/**
 Contains the metrics associated with text after layout.
 */
class TextMetrics {
public:
    TextMetrics() : 
        left(0), 
        top(0), 
        width(0), 
        height(0), 
        width_including_trailing_whitespace(0),
        layout_width(0),
        layout_height(0),
        max_bidi_reordering_depth(0),
        line_count(0) {
    
    }

public:
    /**
     A value that indicates the left-most point of formatted text relative to the 
     layout box, while excluding any glyph overhang.
     */
    float left;

    /**
     A value that indicates the top-most point of formatted text relative to the 
     layout box, while excluding any glyph overhang.
     */
    float top;

    /**
     A value that indicates the width of the formatted text, while ignoring trailing
     whitespace at the end of each line.
     */
    float width;

    /**
     The height of the formatted text. The height of an empty string is set to the 
     same value as that of the default font.
     */
    float height;

    /**
     The width of the formatted text, taking into account the trailing whitespace at
     the end of each line.
     */
    float width_including_trailing_whitespace;

    /**
     The initial width given to the layout. It can be either larger or smaller than 
     the text content width, depending on whether the text was wrapped.
     */
    float layout_width;

    /**
     Initial height given to the layout. Depending on the length of the text, it may 
     be larger or smaller than the text content height.
     */
    float layout_height;

    /**
     The maximum reordering count of any line of text, used to calculate the most number
     of hit-testing boxes needed. If the layout has no bidirectional text, or no text at 
     all, the minimum level is 1.
     */
    std::uint32_t max_bidi_reordering_depth;

    /**
     Total number of lines.
     */
    std::uint32_t line_count;
};

}