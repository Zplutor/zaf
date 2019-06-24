#include <zaf/parsing/parsers/rect_parser.h>
#include <zaf/graphic/rect.h>
#include <zaf/parsing/utility.h>
#include <zaf/parsing/xaml_node.h>
#include <zaf/reflection/reflection_type.h>

namespace zaf {

void RectParser::ParseFromNode(
    const std::shared_ptr<XamlNode>& node, 
    ReflectionObject& reflection_object) {

    auto& rect = dynamic_cast<Rect&>(reflection_object);

    ParseNodeAttributeToObject(node, L"position", rect.position);
    ParseNodeAttributeToObject(node, L"size", rect.size);

    ParseNodePropertyNodeToObject(node, L"Rect.Position", rect.position);
    ParseNodePropertyNodeToObject(node, L"Rect.Size", rect.size);
}

}