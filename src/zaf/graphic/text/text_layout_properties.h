#pragma once

#include <memory>
#include <string>

namespace zaf {

class TextFormat;

class TextLayoutProperties {
public:
	TextLayoutProperties() : width(0), height(0) { }

	std::wstring text;
	std::shared_ptr<TextFormat> text_format;
	float width;
	float height;
};

}