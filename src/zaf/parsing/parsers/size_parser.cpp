#include <zaf/parsing/parsers/size_parser.h>
#include <zaf/graphic/size.h>
#include <zaf/parsing/parsers/internal/utility.h>
#include <zaf/parsing/xaml_node_parse_helper.h>

namespace zaf {

void SizeParser::ParseFromAttribute(
    const std::wstring& attribute_value,
    Object& reflection_object) {

    auto& size = dynamic_cast<Size&>(reflection_object);
    internal::ParseAttributeToDoubleFloats(attribute_value, size.width, size.height);
}


void SizeParser::ParseFromNode(const XamlNode& node, Object& reflection_object) {

    Size& size = dynamic_cast<Size&>(reflection_object);
    XamlNodeParseHelper helper(node, size.GetType());

    auto width = helper.GetFloatProperty(L"Width");
    if (width) {
        size.width = *width;
    }
    
    auto height = helper.GetFloatProperty(L"Height");
    if (height) {
        size.height = *height;
    }
}

}