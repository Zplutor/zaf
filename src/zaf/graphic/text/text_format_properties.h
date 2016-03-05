#pragma once

#include <memory>
#include <string>

namespace zaf {

class FontCollection;

class TextFormatProperties {
public:
	TextFormatProperties() : 
		font_size(0), 
		font_weight(0) {
	
	}

	std::shared_ptr<FontCollection> font_collection;
	std::wstring font_family_name;
	float font_size;
	int font_weight;
};

}