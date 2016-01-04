#pragma once

#include <cstdint>
#include <zaf/base/direct2d.h>

namespace zaf {

/**
 Describes the red, green, blue, and alpha components of a color.
 */
class Color {
public:
	/**
	 Convert a specified D2D1_COLOR_F structure to Color.
	 */
	static Color FromD2D1COLORF(const D2D1_COLOR_F& color) {
		return Color(color.r, color.g, color.b, color.a);
	}

	/**
	 Convert a specified RGB value to Color, which alpha is 1.
	 */
	static Color FromRGB(std::uint32_t rgb) {
		return FromRGB(rgb, 1);
	}

	/**
	 Convert specified RGB value and alpha to Color.
	 */
	static Color FromRGB(std::uint32_t rgb, float alpha) {

		float r = ((rgb & 0x00ff0000) >> 16) / 255.f;
		float g = ((rgb & 0x0000ff00) >> 8) / 255.f;
		float b = ((rgb & 0x000000ff)) / 255.f;
		return Color(r, g, b, alpha);
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

}