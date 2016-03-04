#pragma once

#include <string>
#include <zaf/graphic/text/font_weight.h>
#include <zaf/graphic/text/paragraph_alignment.h>
#include <zaf/graphic/text/text_alignment.h>
#include <zaf/graphic/text/word_wrapping.h>

namespace zaf {

/**
 Describes properties of how to display text in control.
 */
class TextProperties {
public:
	/**
	 Initialize the instance.
	 */
	TextProperties() : 
		font_size(14),
		font_weight(FontWeight::Regular),
		text_alignment(TextAlignment::Leading),
		paragraph_alignment(ParagraphAlignment::Near),
		word_wrapping(WordWrapping::NoWrap) {
	
	}

	/**
	 Font family name.

	 The default family name is empty, means the default font family name
	 of system.
	 */
	std::wstring font_family_name;

	/**
	 Font size.

	 The default size is 14.
	 */
	float font_size;

	/**
 	 Font weight.

	 A valid value is between 1 and 999. There are predefined values
	 contained in class FontWeight. The default font weight value is
	 FontWeight::Regular.
	 */
	int font_weight;

	/**
	 Text alignment in horizontal.

	 The default value is TextAlignment::Leading.
	 */
	TextAlignment text_alignment;

	/**
	 Paragraph alignment in vertical.

	 The default value is ParagraphAlignment::Near.
	 */
	ParagraphAlignment paragraph_alignment;

	/**
	 Word wrapping.

	 The default value is WordWrapping::NoWrap.
	 */
	WordWrapping word_wrapping;
};

}