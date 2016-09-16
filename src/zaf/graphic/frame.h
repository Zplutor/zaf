#pragma once

#include <Windows.h>

namespace zaf {

/**
 Represents a rectangle frame.   
 */
class Frame {
public:
    /**
     Convert a RECT structure to Frame.
     */
    static const Frame FromRECT(const RECT& rect) {
        return Frame(
            static_cast<float>(rect.left),
            static_cast<float>(rect.top),
            static_cast<float>(rect.bottom),
            static_cast<float>(rect.bottom));
    }

public:
    /**
     Construct an empty frame.
     */
    Frame() : left(0), top(0), right(0), bottom(0) { }

    /**
     Construct a frame with specified thicknesses.
     */
    Frame(float left, float top, float right, float bottom) : 
        left(left),
        top(top),
        right(right), 
        bottom(bottom) { }

    /**
     Convert to a RECT structure.
     */
    const RECT ToRECT() const {
        RECT rect;
        rect.left = static_cast<LONG>(left);
        rect.top = static_cast<LONG>(top);
        rect.right = static_cast<LONG>(right);
        rect.bottom = static_cast<LONG>(bottom);
        return rect;
    }

public:
    /**
     Left edge thickness.
     */
    float left;

    /**
     Top edge thickness.
     */
    float top;

    /**
     Right edge thickness.
     */
    float right;

    /**
     Bottom edge thickness.
     */
    float bottom;
};

}