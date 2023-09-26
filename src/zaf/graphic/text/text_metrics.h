#pragma once

#include <cstdint>

namespace zaf {

/**
 Contains the metrics associated with text after layout.
 */
class TextMetrics {
public:
    /**
     A value that indicates the left-most point of formatted text relative to the 
     layout box, while excluding any glyph overhang.
     */
    float Left() const {
        return inner_.left;
    }

    /**
     A value that indicates the top-most point of formatted text relative to the 
     layout box, while excluding any glyph overhang.
     */
    float Top() const {
        return inner_.top;
    }

    /**
     A value that indicates the width of the formatted text, while ignoring trailing
     whitespace at the end of each line.
     */
    float Width() const {
        return inner_.width;
    }

    /**
     The height of the formatted text. The height of an empty string is set to the 
     same value as that of the default font.
     */
    float Height() const {
        return inner_.height;
    }

    /**
     The width of the formatted text, taking into account the trailing whitespace at
     the end of each line.
     */
    float WidthIncludingTrailingWhitespace() const {
        return inner_.widthIncludingTrailingWhitespace;
    }

    float GetWidth(bool ignore_trailing_white_spaces) const {

        if (ignore_trailing_white_spaces) {
            return Width();
        }

        //Note that if TextAlignment is set to Center, WidthIncludingTrailingWhitespace would be 0.
        return
            WidthIncludingTrailingWhitespace() != 0 ?
            WidthIncludingTrailingWhitespace() :
            Width();
    }

    /**
     The initial width given to the layout. It can be either larger or smaller than 
     the text content width, depending on whether the text was wrapped.
     */
    float LayoutWidth() const {
        return inner_.layoutWidth;
    }

    /**
     Initial height given to the layout. Depending on the length of the text, it may 
     be larger or smaller than the text content height.
     */
    float LayoutHeight() const {
        return inner_.layoutHeight;
    }

    /**
     The maximum reordering count of any line of text, used to calculate the most number
     of hit-testing boxes needed. If the layout has no bidirectional text, or no text at 
     all, the minimum level is 1.
     */
    std::uint32_t MaxBidiReorderingDepth() const {
        return inner_.maxBidiReorderingDepth;
    }

    /**
     Total number of lines.
     */
    std::uint32_t LineCount() const {
        return inner_.lineCount;
    }

    const DWRITE_TEXT_METRICS& Inner() const {
        return inner_;
    }

    DWRITE_TEXT_METRICS& Inner() {
        return inner_;
    }

private:
    DWRITE_TEXT_METRICS inner_{};
};

}