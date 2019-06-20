#include <zaf/parsing/parsers/rect_parser.h>
#include <zaf/graphic/rect.h>
#include <zaf/parsing/parsers/internal/utility.h>
#include <zaf/parsing/utility.h>
#include <zaf/parsing/xaml_node.h>
#include <zaf/reflection/reflection_type.h>

namespace zaf {

void RectParser::ParseFromAttribute(
    const std::wstring& attribute_value,
    ReflectionObject& reflection_object) {

    auto& rect = dynamic_cast<Rect&>(reflection_object);
    internal::ParseAttributeToQuaterFloats(
        attribute_value, 
        rect.position.x,
        rect.position.y,
        rect.size.width,
        rect.size.height);
}


void RectParser::ParseFromNode(
    const std::shared_ptr<XamlNode>& node, 
    ReflectionObject& reflection_object) {

    auto& rect = dynamic_cast<Rect&>(reflection_object);

    ParseNodeAttributeToObject(node, L"position", rect.position);
    ParseNodeAttributeToObject(node, L"size", rect.size);

    ParseNodePropertyNodetoObject(node, L"Rect.Position", rect.position);
    ParseNodePropertyNodetoObject(node, L"Rect.Size", rect.size);
}

}