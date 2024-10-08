#pragma once

#include <cstdint>
#include <zaf/base/direct2d.h>
#include <zaf/base/hash.h>
#include <zaf/base/relation_operator.h>
#include <zaf/graphic/color_parser.h>
#include <zaf/object/equality.h>
#include <zaf/object/object.h>
#include <zaf/object/property_support.h>
#include <zaf/xml/xml_serializable.h>

namespace zaf {

/**
 Describes the red, green, blue, and alpha components of a color.
 */
class Color : public Object, public XMLSerializable {
public:
    ZAF_OBJECT;
    ZAF_DECLARE_EQUALITY;

public:
    /**
     Convert a specified D2D1_COLOR_F structure to Color.
     */
    static Color FromD2D1COLORF(const D2D1_COLOR_F& color) {
        return Color(color.r, color.g, color.b, color.a);
    }

    /**
     Convert a specified ARGB value, which in format 0xAARRGGBB, to Color.
     */
    static Color FromARGB(std::uint32_t argb) {
        float alpha = float(argb >> 24) / 255;
        return FromRGB(argb, alpha);
    }

    /**
     Convert a specified RGB value, which in format 0x00RRGGBB, to Color, 
     which alpha is 1.
     */
    static Color FromRGB(std::uint32_t rgb) {
        return FromRGB(rgb, 1);
    }

    /**
     Convert specified RGB value, which in format 0x00RRGGBB, and specified 
     alpha to Color.
     */
    static Color FromRGB(std::uint32_t rgb, float alpha) {
        return FromD2D1COLORF(D2D1::ColorF(rgb, alpha));
    }

    /**
     Convert specified COLORREF value to Color.
     */
    static Color FromCOLORREF(COLORREF color) {
        return Color(
            static_cast<float>(GetRValue(color)) / 255,
            static_cast<float>(GetGValue(color)) / 255,
            static_cast<float>(GetBValue(color)) / 255
        );
    }

public:
    /**
     A color which RGB is 0, 0, 0, and alpha is 1.
     */
    static const Color& Black();

    /**
      A color which RGB is 0, 0, 1, and alpha is 1.
     */
    static const Color& Blue();

    /**
     A color which RGB is 0, 1, 1, and alpha is 1.
     */
    static const Color& Cyan();

    /**
     A color which RGB is 0.5, 0.5, 0.5, and alpha is 1.
     */
    static const Color& Gray();

    /**
     A color which RGB is 0, 0.5, 0, and alpha is 1.
     */
    static const Color& Green();

    /**
     A color which RGB is 0, 1, 0, and alpha is 1.
     */
    static const Color& Lime();

    /**
     A color which RGB is 1, 0, 1, and alpha is 1.
     */
    static const Color& Magenta();

    /**
     A color which RGB is 1, 0, 0, and alpha is 1.
     */
    static const Color& Red();

    /**
     A color which RGB is 0, 0, 0, and alpha is 0.
     */
    static const Color& Transparent();

    /**
     A color which RGB is 1, 1, 1, and alpha is 1.
     */
    static const Color& White();

    /**
     A color which RGB is 1, 1, 0, and alpha is 1.
     */
    static const Color& Yellow();

public:
    /**
     Initialize the instance which red, green, and blue are zero, and alpha is 1.
     */
    Color() : r(0), g(0), b(0), a(1) { }

    /**
     Initialize the instance that is the same as another Color.
     */
    Color(const Color& other) : r(other.r), g(other.g), b(other.b), a(other.a) { }

    /**
     Initialize the instance that has specified red, green, blue, and 1 alpha.
     */
    Color(float r, float g, float b) : r(r), g(g), b(b), a(1) { }

    /**
     Initialize the instance that has specified red, green, blue and alpha.
     */
    Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) { }

    /**
     Assign from another Color instance.
     */
    Color& operator=(const Color& other) {

        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
        return *this;
    }

    /**
     Convert to D2D1_COLOR_F structure.
     */
    D2D1_COLOR_F ToD2D1COLORF() const {

        D2D1_COLOR_F color;
        color.r = r;
        color.g = g;
        color.b = b;
        color.a = a;
        return color;
    }

    /**
     Convert to COLORREF value.

     Note that the alpha component of the color will be discarded in COLORREF.
     */
    COLORREF ToCOLORREF() const {
        return RGB(r * 255, g * 255, b * 255);
    }

    std::wstring ToString() const override;

    void WriteToXML(XMLWriter& writer) const override;
    void ReadFromXML(XMLReader& reader) override;

    float R() const {
        return r;
    }

    void SetR(float r) {
        this->r = r;
    }

    float G() const {
        return g;
    }

    void SetG(float g) {
        this->g = g;
    }

    float B() const {
        return b;
    }

    void SetB(float b) {
        this->b = b;
    }

    float A() const {
        return a;
    }

    void SetA(float a) {
        this->a = a;
    }

public:
    /**
     Red component of the color.
     */
    float r;

    /**
     Green component of the color.
     */
    float g;

    /**
     Blue component of the color.
     */
    float b;

    /**
     Alpha component of the color.
     */
    float a;
};

ZAF_OBJECT_BEGIN(Color);
ZAF_OBJECT_PARSER(ColorParser);
ZAF_OBJECT_PROPERTY(R);
ZAF_OBJECT_PROPERTY(G);
ZAF_OBJECT_PROPERTY(B);
ZAF_OBJECT_PROPERTY(A);
ZAF_OBJECT_END;


inline bool operator==(const Color& color1, const Color& color2) {
    return 
        (color1.a == color2.a) &&
        (color1.r == color2.r) &&
        (color1.g == color2.g) &&
        (color1.b == color2.b);
}


inline bool operator<(const Color& color1, const Color& color2) {
    
    if (color1.a < color2.a) {
        return true;
    }

    if (color1.r < color2.r) {
        return true;
    }

    if (color1.g < color2.g) {
        return true;
    }

    return color1.b < color2.b;
}


ZAF_DEFINE_RELATION_OPERATORS(Color);

}


namespace std {
template<>
struct hash<zaf::Color> {
    std::size_t operator()(const zaf::Color& color) {
        return zaf::CalculateHash(color.r, color.g, color.b, color.a);
    }
};
}