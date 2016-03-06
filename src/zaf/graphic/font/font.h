#pragma once

#include <string>
#include <zaf/graphic/font/font_weight.h>

namespace zaf {

/**
 Describes various properties of a font.
 */
class Font {
public:
	/**
	 Initialize the instance.
	 */
	Font() :
		size(14),
		weight(FontWeight::Regular) {

	}

	/**
	 Font family name.

	 The default family name is empty, means the default font family name
	 of system.
	 */
	std::wstring family_name;

	/**
	 Font size.

	 The default size is 14.
	 */
	float size;

	/**
	 Font weight.

	 A valid value is between 1 and 999. There are predefined values
	 contained in class FontWeight. The default font weight value is
	 FontWeight::Regular.
	 */
	int weight;
};

}