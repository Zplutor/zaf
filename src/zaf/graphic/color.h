#pragma once

#include <cstdint>
#include <zaf/base/direct2d.h>

namespace zaf {

/**
 Represent a sRBG color.	
 */
class Color {
public:
	static Color FromD2D1COLORF(const D2D1_COLOR_F& color) {
		return Color(color.r, color.g, color.b, color.a);
	}

public:
	static const Color Black;
	static const Color Blue;
	static const Color Cyan;
	static const Color Gray;
	static const Color Green;
	static const Color Lime;
	static const Color Magenta;
	static const Color Red;
	static const Color White;
	static const Color Yellow;

public:
	Color() : r(0), g(0), b(0), a(0) { }

	Color(const Color& other) : r(other.r), g(other.g), b(other.b), a(other.a) { }

	Color(float r, float g, float b) : r(r), g(g), b(b), a(1) { }
	Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) { }

	Color(std::int32_t rgb) : a(1) {
		InitializeWithRgb(rgb);
	}

	Color(std::int32_t rgb, float a) : a(a) {
		InitializeWithRgb(rgb);
	}

	Color& operator=(const Color& other) {

		r = other.r;
		g = other.g;
		b = other.b;
		a = other.a;
		return *this;
	}

	D2D1_COLOR_F ToD2D1COLORF() const {

		D2D1_COLOR_F color;
		color.r = r;
		color.g = g;
		color.b = b;
		color.a = a;
		return color;
	}

public:
	float r;
	float g;
	float b;
	float a;

private:
	void InitializeWithRgb(std::int32_t rgb) {

		r = ((rgb & 0x00ff0000) >> 16) / 255.f;
		g = ((rgb & 0x0000ff00) >> 8) / 255.f;
		b = ((rgb & 0x000000ff)) / 255.f;
	}
};

}