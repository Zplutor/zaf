#pragma once

#include <memory>
#include <string>
#include <zaf/graphic/font/font_weight.h>

namespace zaf {

class FontCollection;

/**
 Describes properties used to create a text format.
 */
class TextFormatProperties {
public:
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
     Locale name.
     */
    std::wstring locale_name;

    /**
     Font size.
     */
    float font_size{ 14 };

    /**
     Font weight.
     */
    int font_weight{ FontWeight::Regular };
};

}