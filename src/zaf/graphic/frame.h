#pragma once

#include <Windows.h>
#include <zaf/base/hash.h>
#include <zaf/base/relation_operator.h>
#include <zaf/object/equality_type.h>
#include <zaf/reflection/reflection_object.h>

namespace zaf {

/**
 Represents a rectangle frame.   
 */
class Frame : public ReflectionObject {
public:
    ZAF_DECLARE_EQUALITY_TYPE
    ZAF_DECLARE_REFLECTION_TYPE

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
     Construct a frame which all direction is the same as specified thickness.
     */
    Frame(float all) : left(all), top(all), right(all), bottom(all) { }

    /**
     Construct a frame with specified thicknesses.
     */
    Frame(float left, float top, float right, float bottom) : 
        left(left),
        top(top),
        right(right), 
        bottom(bottom) { }

    bool IsEmpty() const {

        return 
            (left == 0) &&
            (top == 0) &&
            (right == 0) &&
            (bottom == 0);
    }

    /**
     Convert to a RECT structure.
     */
    RECT ToRECT() const {
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


inline bool operator==(const Frame& frame1, const Frame& frame2) {
    return
        (frame1.left == frame2.left) &&
        (frame1.top == frame2.top) &&
        (frame1.right == frame2.right) &&
        (frame1.bottom == frame2.bottom);
}


inline bool operator<(const Frame& frame1, const Frame& frame2) {

    if (frame1.left < frame2.left) {
        return true;
    }

    if (frame1.top < frame2.top) {
        return true;
    }

    if (frame1.right < frame2.right) {
        return true;
    }

    return frame1.bottom < frame2.bottom;
}


ZAF_DEFINE_RELATION_OPERATORS(Frame);

}


namespace std {
template<>
struct hash<zaf::Frame> {
    std::size_t operator()(const zaf::Frame& frame) {
        return zaf::CalculateHash(frame.left, frame.top, frame.right, frame.bottom);
    }
};
}