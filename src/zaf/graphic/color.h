#pragma once

#include <cstdint>
#include <zaf/base/direct2d.h>
#include <zaf/base/relation_operator.h>

namespace zaf {

/**
 Describes the red, green, blue, and alpha components of a color.
 */
class Color {
public:
	/**
	 Convert a specified D2D1_COLOR_F structure to Color.
	 */
	static const Color FromD2D1COLORF(const D2D1_COLOR_F& color) {
		return Color(color.r, color.g, color.b, color.a);
	}

	/**
	 Convert a specified RGB value to Color, which alpha is 1.
	 */
	static const Color FromRGB(std::uint32_t rgb) {
		return FromRGB(rgb, 1);
	}

	/**
	 Convert specified RGB value and alpha to Color.
	 */
	static const Color FromRGB(std::uint32_t rgb, float alpha) {
        return FromD2D1COLORF(D2D1::ColorF(rgb, alpha));
	}

    /**
     Convert specified COLORREF value to Color.
     */
    static const Color FromCOLORREF(COLORREF color) {
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
	static const Color Black;

	/**
 	 A color which RGB is 0, 0, 1, and alpha is 1.
	 */
	static const Color Blue;

	/**
	 A color which RGB is 0, 1, 1, and alpha is 1.
	 */
	static const Color Cyan;

	/**
	 A color which RGB is 0.5, 0.5, 0.5, and alpha is 1.
	 */
	static const Color Gray;

	/**
	 A color which RGB is 0, 0.5, 0, and alpha is 1.
	 */
	static const Color Green;

	/**
	 A color which RGB is 0, 1, 0, and alpha is 1.
	 */
	static const Color Lime;

	/**
	 A color which RGB is 1, 0, 1, and alpha is 1.
	 */
	static const Color Magenta;

	/**
	 A color which RGB is 1, 0, 0, and alpha is 1.
	 */
	static const Color Red;

	/**
	 A color which RGB is 0, 0, 0, and alpha is 0.
	 */
	static const Color Transparent;

	/**
	 A color which RGB is 1, 1, 1, and alpha is 1.
	 */
	static const Color White;

	/**
	 A color which RGB is 1, 1, 0, and alpha is 1.
	 */
	static const Color Yellow;

public:
	/**
	 Initialize the instance which red, green, blue and alpha are all zero.
	 */
	Color() : r(0), g(0), b(0), a(0) { }

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


ZAF_ENABLE_RELATION_OPERATOR(Color);

}