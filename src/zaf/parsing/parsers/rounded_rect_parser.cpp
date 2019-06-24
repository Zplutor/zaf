#include <zaf/parsing/parsers/rounded_rect_parser.h>
#include <zaf/graphic/rounded_rect.h>
#include <zaf/parsing/utility.h>
#include <zaf/parsing/xaml_node.h>
#include <zaf/reflection/reflection_type.h>

namespace zaf {

void RoundedRectParser::ParseFromNode(
    const std::shared_ptr<XamlNode>& node,
    ReflectionObject& reflection_object) {

    auto& rounded_rect = dynamic_cast<RoundedRect&>(reflection_object);
    
    ParseNodeAttributeToFloat(node, L"xRadius", rounded_rect.x_radius);
    ParseNodeAttributeToFloat(node, L"yRadius", rounded_rect.y_radius);

    ParseNodePropertyNodeToFloat(node, L"RoundedRect.XRadius", rounded_rect.x_radius);
    ParseNodePropertyNodeToFloat(node, L"RoundedRect.YRadius", rounded_rect.y_radius);

    ParseNodePropertyNodeToObject(node, L"RoundedRect.Rect", rounded_rect.rect);
}

}