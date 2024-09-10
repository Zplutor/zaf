#include <zaf/graphic/font_parser.h>
#include <zaf/graphic/font.h>

namespace zaf {

void FontParser::ParseFromNode(const XamlNode& node, Object& object) {

    //We want the fonts used in XAML have the same properties as the default font,
    //so we assign the default font to the object before parsing.
    auto& font = dynamic_cast<Font&>(object);
    font = Font::Default();

    __super::ParseFromNode(node, object);
}

}