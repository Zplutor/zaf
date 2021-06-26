#include <zaf/parsing/parsers/rect_parser.h>
#include <zaf/graphic/rect.h>
#include <zaf/parsing/xaml_node.h>
#include <zaf/parsing/xaml_node_parse_helper.h>

namespace zaf {

void RectParser::ParseFromNode(
    const XamlNode& node,
 
    Object& reflection_object) {

    auto& rect = dynamic_cast<Rect&>(reflection_object);
    XamlNodeParseHelper helper(node, rect.GetType());

    auto position = helper.GetObjectProperty<Point>(L"Position");
    if (position) {
        rect.position = *position;
    }

    auto size = helper.GetObjectProperty<Size>(L"Size");
    if (size) {
        rect.size = *size;
    }
}

}