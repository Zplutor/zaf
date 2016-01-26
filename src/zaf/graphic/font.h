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
		size_(14), 
		weight_(Weight::Regular), 
		style_(Style::Regular) { 
	
	}

	/**
	 Initialize the instance that has the same values as another.
	 */
	Font(const Font& other) :
		family_name_(other.family_name_),
		size_(other.size_),
		weight_(other.weight_),
		style_(other.style_) {

	}

	/**
	 Assign the font from another.
	 */
	Font& operator=(const Font& other) {

		family_name_ = other.family_name_;
		size_ = other.size_;
		weight_ = other.weight_;
		style_ = other.style_;
	}

	/**
	 Get the font family name.

	 See also SetFamilyName.
	 */
	const std::wstring& GetFamilyName() const {
		return family_name_;
	}

	/**
	 Set the font family name.

	 The default family name is empty, means which font family is used
	 is decided by the system.
	 */
	void SetFamilyName(const std::wstring& family_name) {
		family_name_ = family_name;
	}

	/**
	 Get the font size.

	 See also SetFont.
	 */
	float GetSize() const {
		return size_;
	}

	/**
	 Set the font size.

	 The default size is 14.
	 */
	void SetSize(float size) {
		size_ = size;
	}

	/**
	 Get the font weight.

	 See also SetWeight.
	 */
	int GetWeight() const {
		return weight_;
	}

	/**
	 Set the font weight.

	 A valid value is between 1 and 999. There are predefined values 
	 contained in class Weight. The default font weight value is 
	 Weight::Regular.
  	 */
	void SetWeight(int weight) {
		weight_ = weight;
	}

	/**
	 Get the font style.

	 See also SetStyle.
	 */
	Style GetStyle() const {
		return style_;
	}

	/**
	 Set the font style.

	 The default value is Style::Regular.
	 */
	void SetStyle(Style style) {
		style_ = style;
	}

private:
	std::wstring family_name_;
	float size_;
	int weight_;
	Style style_;
};

ZAF_ENABLE_FLAG_ENUM(Font::Style);

}