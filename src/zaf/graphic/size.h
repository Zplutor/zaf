#pragma once

#include <memory>
#include <zaf/base/direct2d.h>
#include <zaf/base/hash.h>
#include <zaf/base/relation_operator.h>
#include <zaf/graphic/size_parser.h>
#include <zaf/dynamic/equality.h>
#include <zaf/dynamic/object.h>
#include <zaf/dynamic/property_support.h>

namespace zaf {

/**
 Describes the width and height of a size.    
 */
class Size : public Object {
public:
    ZAF_OBJECT;
    ZAF_DECLARE_EQUALITY;

public:
    /**
     Convert a specified D2D1_SIZE_F structure to Size.
     */
    static Size FromD2D1SIZEF(const D2D1_SIZE_F& size) {
        return Size(size.width, size.height);
    }

    /**
     Convert a specified D2D1_SIZE_U structure to Size.
     */
    static Size FromD2D1SIZEU(const D2D1_SIZE_U& size) {

        return Size(
            static_cast<float>(size.width),
            static_cast<float>(size.height)
        );
    }

public:
    /**
     Initialize the instance that has zero width and height.
     */
    Size() : width(0), height(0) { }

    /**
     Initialize the instance that is the same as another Size. 
     */
    Size(const Size& other) : width(other.width), height(other.height) { }

    /**
     Initialize the instance that has specified width and height. 
     */
    Size(float width, float height) : width(width), height(height) { }

    /**
     Assign from another Size instance.
     */
    Size& operator=(const Size& other) {

        width = other.width;
        height = other.height;
        return *this;
    }

    /**
    Convert to SIZE structure.
    */
    SIZE ToSIZE() const {

        SIZE result;
        result.cx = static_cast<LONG>(width);
        result.cy = static_cast<LONG>(height);
        return result;
    }

    /**
    Convert to SIZEL structure.
    */
    SIZEL ToSIZEL() const {

        //SIZEL is the same as SIZE.
        return ToSIZE();
    }

    /**
     Convert to D2D1_SIZE_F structure.
     */
    D2D1_SIZE_F ToD2D1SIZEF() const {

        D2D1_SIZE_F size;
        size.width = width;
        size.height = height;
        return size;
    }

    /**
     Convert to D2D1_SIZE_U structure.
     */
    D2D1_SIZE_U ToD2D1SIZEU() const {

        D2D1_SIZE_U size;
        size.width = static_cast<UINT32>(width);
        size.height = static_cast<UINT32>(height);
        return size;
    }

    std::wstring ToString() const override;

    float Width() const {
        return width;
    }

    void SetWidth(float width) {
        this->width = width;
    }

    float Height() const {
        return height;
    }

    void SetHeight(float height) {
        this->height = height;
    }

public:
    /**
     Width of the size.
     */
    float width;

    /**
     Height of the size.
     */
    float height;
};

ZAF_OBJECT_BEGIN(Size);
ZAF_OBJECT_PARSER(SizeParser);
ZAF_OBJECT_PROPERTY(Width);
ZAF_OBJECT_PROPERTY(Height);
ZAF_OBJECT_END;


inline bool operator==(const Size& size1, const Size& size2) {
    return (size1.width == size2.width) && (size1.height == size2.height);
}

inline bool operator<(const Size& size1, const Size& size2) {

    if (size1.width < size2.width) {
        return true;
    }

    if (size1.width > size2.width) {
        return false;
    }

    return size1.height < size2.height;
}

ZAF_DEFINE_RELATION_OPERATORS(Size);

}


namespace std {
template<>
struct hash<zaf::Size> {
    std::size_t operator()(const zaf::Size& size) {
        return zaf::CalculateHash(size.width, size.height);
    }
};
}