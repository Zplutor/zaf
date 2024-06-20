#pragma once

#include <Windows.h>
#include <zaf/base/hash.h>
#include <zaf/base/relation_operator.h>
#include <zaf/object/equality.h>
#include <zaf/object/object.h>
#include <zaf/object/property_support.h>
#include <zaf/graphic/frame_parser.h>

namespace zaf {

/**
 Represents a rectangle frame.   
 */
class Frame : public Object {
public:
    ZAF_OBJECT;
    ZAF_DECLARE_EQUALITY

public:
    /**
     Convert a RECT structure to Frame.
     */
    static const Frame FromRECT(const RECT& rect) {
        return Frame(
            static_cast<float>(rect.left),
            static_cast<float>(rect.top),
            static_cast<float>(rect.right),
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
    explicit Frame(float all) : left(all), top(all), right(all), bottom(all) { }

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

    float Width() const {
        return left + right;
    }

    float Height() const {
        return top + bottom;
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

    std::wstring ToString() const override;

    float Left() const {
        return left;
    }

    void SetLeft(float left) {
        this->left = left;
    }

    float Top() const {
        return top;
    }

    void SetTop(float top) {
        this->top = top;
    }

    float Right() const {
        return right;
    }

    void SetRight(float right) {
        this->right = right;
    }

    float Bottom() const {
        return bottom;
    }

    void SetBottom(float bottom) {
        this->bottom = bottom;
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

ZAF_OBJECT_BEGIN(Frame);
ZAF_OBJECT_PARSER(FrameParser);
ZAF_PROPERTY(Left)
ZAF_PROPERTY(Top)
ZAF_PROPERTY(Right)
ZAF_PROPERTY(Bottom)
ZAF_PROPERTY(Width)
ZAF_PROPERTY(Height)
ZAF_OBJECT_END;


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