#pragma once

#include <memory>
#include <string>

namespace zaf {

class FontCollection;

/**
 Describes properties used to create a text format.
 */
class TextFormatProperties {
public:
	TextFormatProperties() : 
		font_size(0), 
		font_weight(0) {
	
	}

    /**
     Font collection instance.

     If this field is nullptr, indicates the system font collection.
     */
	std::shared_ptr<FontCollection> font_collection;

    /**
     Font family name.
     */
	std::wstring font_family_name;

    /**
     Font size.
     */
	float font_size;

    /**
     Font weight.
     */
	int font_weight;
};

}