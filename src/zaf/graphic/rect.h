#pragma once

#include <Windows.h>
#include <Wincodec.h>
#include <zaf/base/direct2d.h>
#include <zaf/base/hash.h>
#include <zaf/base/relation_operator.h>
#include <zaf/graphic/frame.h>
#include <zaf/graphic/point.h>
#include <zaf/graphic/size.h>

namespace zaf {

/**
 Describes the position and size of a rectangle area.
 */
class Rect : public Object {
public:
    ZAF_DECLARE_TYPE
    ZAF_DECLARE_EQUALITY

public:
    /**
     Convert a specified RECT structure to Rect.
     */
    static Rect FromRECT(const RECT& rect) {

        return Rect(
            static_cast<float>(rect.left),
            static_cast<float>(rect.top),
            static_cast<float>(rect.right - rect.left),
            static_cast<float>(rect.bottom - rect.top)
        );
    }

    /**
     Convert a specified D2D1_RECT_F structure to Rect.
     */
    static Rect FromD2D1RECTF(const D2D1_RECT_F& rect) {

        return Rect(
            rect.left,
            rect.top,
            rect.right - rect.left,
            rect.bottom - rect.top
        );
    }

    /**
     Convert a specified D2D1_RECT_U structure to Rect.
     */
    static Rect FromD2D1RECTU(const D2D1_RECT_U& rect) {

        return Rect(
            static_cast<float>(rect.left),
            static_cast<float>(rect.top),
            static_cast<float>(rect.right - rect.left),
            static_cast<float>(rect.bottom - rect.top)
        );
    }

    /**
     Create a Rect instance that has the intersection rectangle of other two instances.
     */
    static Rect Intersect(const Rect& rect1, const Rect& rect2) {
        Rect result = rect1;
        result.Intersect(rect2);
        return result;
    }

    /**
     Create a Rect instance that has the union rectangle of other two instances.
     */
    static Rect Union(const Rect& rect1, const Rect& rect2) {
        Rect result = rect1;
        result.Union(rect2);
        return result;
    }

    static Rect Subtract(const Rect& rect1, const Rect& rect2) {
        Rect result = rect1;
        result.Subtract(rect2);
        return result;
    }

    /**
     The infinite rectangle.
     */
    static const Rect Infinite;

public:
    /**
     Initialize the instance that has an empty rectangle.
     */
    Rect() { }

    /**
     Initialize the instance that is the same as another Rect.
     */
    Rect(const Rect& other) : position(other.position), size(other.size) { }

    /**
     Initialize the instance that has specified position and size.
     */
    Rect(const Point& position, const Size& size) : position(position), size(size) { }

    /**
     Initialize the instance that has specified x-coordinate, y-coordinate, width and height.
       */
    Rect(float x, float y, float width, float height) : position(x, y), size(width, height) { }

    /**
     Assign from another Rect instance.
     */
    Rect& operator=(const Rect& other) {
        position = other.position;
        size = other.size;
        return *this;
    }

    /**
     Determine whether the rectangle has intersection with another Rect.
     */
    bool HasIntersection(const Rect& other) const {
        Rect result = Intersect(*this, other);
        return !result.IsEmpty();
    }

    /**
     Make an intersection rectangle with another Rect.
     */
    void Intersect(const Rect& other);

    /**
     Make an union rectangle with another Rect.
     */
    void Union(const Rect& other);

    void Subtract(const Rect& other);

    /**
     Infalte the rectangle with specified size.
     */
    void Inflate(float size) {
        Inflate(size, size);
    }

    /**
     Infalte the rectangle with specified width and height.
     */
    void Inflate(float width, float height) {

        position.x -= width;
        position.y -= height;
        size.width += width * 2;
        size.height += height * 2;
    }

    void Inflate(const Frame& frame) {

        position.x -= frame.left;
        position.y -= frame.top;
        size.width += frame.left + frame.right;
        size.height += frame.top + frame.bottom;
    }

    void Deflate(const Frame& frame) {

        position.x += frame.left;
        position.y += frame.top;
        size.width -= frame.left + frame.right;
        size.height -= frame.top + frame.bottom;
    }

    void AddOffset(const Point& offset) {
        position.AddOffset(offset);
    }

    void SubtractOffset(const Point& offset) {
        position.SubtractOffset(offset);
    }

    /**
     Determine whether the specified point locates within the rectangle.
     */
    bool Contain(const Point& point) const {

        return 
            (point.x >= position.x) && 
            (point.x < position.x + size.width) &&
            (point.y >= position.y) &&
            (point.y < position.y + size.height);
    }

    /**
     Determine whether the ractangle is empty.
     */
    bool IsEmpty() const {

        return 
            (position.x == 0) && 
            (position.y == 0) && 
            (size.width == 0) && 
            (size.height == 0);
    }

    /**
     Determine whether the rectangle is infinite.
     */
    bool IsInfinite() const;

    /**
     Convert to RECT structure.
     */
    RECT ToRECT() const {

        RECT rect;
        rect.left = static_cast<LONG>(position.x);
        rect.top = static_cast<LONG>(position.y);
        rect.right = static_cast<LONG>(position.x + size.width);
        rect.bottom = static_cast<LONG>(position.y + size.height);
        return rect;
    }
    
    /**
     Convert to D2D1_RECT_F structure.
     */
    D2D1_RECT_F ToD2D1RECTF() const{

        D2D1_RECT_F rect;
        rect.left = position.x;
        rect.top = position.y;
        rect.right = position.x + size.width;
        rect.bottom = position.y + size.height;
        return rect;
    }

    /**
     Convert to D2D1_RECT_U structure.
     */
    D2D1_RECT_U ToD2D1RECTU() const {

        D2D1_RECT_U rect;
        rect.left = static_cast<UINT32>(position.x);
        rect.top = static_cast<UINT32>(position.y);
        rect.right = static_cast<UINT32>(position.x + size.width);
        rect.bottom = static_cast<UINT32>(position.y + size.height);
        return rect;
    }

    /**
     Convert to WICRect structure.
     */
    WICRect ToWICRect() const {

        WICRect wic_rect;
        wic_rect.X = static_cast<INT>(position.x);
        wic_rect.Y = static_cast<INT>(position.y);
        wic_rect.Width = static_cast<INT>(size.width);
        wic_rect.Height = static_cast<INT>(size.height);
        return wic_rect;
    }

    const Point& Position() const {
        return position;
    }

    void SetPosition(const Point& position) {
        this->position = position;
    }

    const Size& Size() const {
        return size;
    }

    void SetSize(const zaf::Size& size) {
        this->size = size;
    }

public:
    /**
     Position of the rectangle.
     */
    Point position;

    /**
     Size of the rectangle.
     */
    zaf::Size size;
};


inline bool operator==(const Rect& rect1, const Rect& rect2) {
    return (rect1.position == rect2.position) && (rect1.size == rect2.size);
}


inline bool operator<(const Rect& rect1, const Rect& rect2) {

    if (rect1.position < rect2.position) {
        return true;
    }

    if (rect1.position > rect2.position) {
        return false;
    }

    return rect1.size < rect2.size;
}


ZAF_DEFINE_RELATION_OPERATORS(Rect);

}


namespace std {
template<>
struct hash<zaf::Rect> {
    std::size_t operator()(const zaf::Rect& rect) {
        return zaf::CalculateHash(rect.position, rect.size);
    }
};
}