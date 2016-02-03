#pragma once

#include <string>
#include <zaf/base/flag_enum.h>

namespace zaf {

/**
 Describes various properties of a font.
 */
class Font {
public:
	/**
	 Contains predefined values of font weight.
	 */
	class Weight {
	public:
		static const int Thin = 100;
		static const int ExtraLight = 200;
		static const int Light = 300;
		static const int SemiLight = 350;
		static const int Regular = 400;
		static const int Medium = 500;
		static const int SemiBold = 600;
		static const int Bold = 700;
		static const int ExtraBold = 800;
		static const int Black = 900;
		static const int ExtraBlack = 950;

	private:
		Weight();
	};

	/**
	 The style of font.
	 */
	enum class Style {
		Regular = 0,
		Italic = 1 << 0,
		Underline = 1 << 1,
	};

public:
	/**
	 Initialize the instance with default values.
	 */
	Font() : 
		size(14), 
		weight(Weight::Regular), 
		style(Style::Regular) { 
	
	}

	/**
	 Initialize the instance that has the same values as another.
	 */
	Font(const Font& other) :
		family_name(other.family_name),
		size(other.size),
		weight(other.weight),
		style(other.style) {

	}

	/**
	 Assign the font from another.
	 */
	Font& operator=(const Font& other) {

		family_name = other.family_name;
		size = other.size;
		weight = other.weight;
		style = other.style;
	}

public:
	/**
	 Font family name.

	 The default family name is empty, means which font family is used
	 is decided by the system.
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
	 contained in class Weight. The default font weight value is
	 Weight::Regular.
	 */
	int weight;

	/**
	 Font style.

	 The default value is Style::Regular.
	 */
	Style style;
};

ZAF_ENABLE_FLAG_ENUM(Font::Style);

}