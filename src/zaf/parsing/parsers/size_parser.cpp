#include <zaf/parsing/parsers/size_parser.h>
#include <zaf/graphic/size.h>
#include <zaf/parsing/parsers/internal/utility.h>
#include <zaf/parsing/utility.h>

namespace zaf {

void SizeParser::ParseFromAttribute(
    const std::wstring& attribute_value,
    ReflectionObject& reflection_object) {

    auto& size = dynamic_cast<Size&>(reflection_object);
    internal::ParseAttributeToDoubleFloats(attribute_value, size.width, size.height);
}


void SizeParser::ParseFromNode(
    const std::shared_ptr<XamlNode>& node, 
    ReflectionObject& reflection_object) {

    Size& size = dynamic_cast<Size&>(reflection_object);

    ParseNodeAttributeToFloat(node, L"width", size.width);
    ParseNodeAttributeToFloat(node, L"height", size.height);

    ParseNodePropertyNodeToFloat(node, L"Size.Width", size.width);
    ParseNodePropertyNodeToFloat(node, L"Size.Height", size.height);
}

}