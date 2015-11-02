#pragma once

#include <cstdint>

namespace zaf {

/**
 Represent a sRBG color.	
 */
class Color {
public:
	Color() : r(1), g(1), b(1), a(1) { }
	Color(float r, float g, float b) : r(r), g(g), b(b), a(1) { }
	Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) { }

	Color(std::int32_t rgb) : a(1) {
		InitializeWithRgb(rgb);
	}

	Color(std::int32_t rgb, float a) : a(a) {
		InitializeWithRgb(rgb);
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