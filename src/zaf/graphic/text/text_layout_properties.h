#pragma once

#include <memory>
#include <string>
#include <zaf/graphic/text/text_format.h>

namespace zaf {

/**
 Describes properties used to create a text layout.
 */
class TextLayoutProperties {
public:
	TextLayoutProperties() : width(0), height(0) { }

    /**
     Text string to be layouted.
     */
	std::wstring text;
    
    /**
     TextFormat object used to format the text.

     This field can not be nullptr.
     */
	TextFormat text_format;

    /**
     Layout width.
     */
	float width;

    /**
     Layout height.
     */
	float height;
};

}