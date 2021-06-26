#include <zaf/parsing/parsers/rounded_rect_parser.h>
#include <zaf/graphic/rounded_rect.h>
#include <zaf/parsing/xaml_node.h>
#include <zaf/parsing/xaml_node_parse_helper.h>

namespace zaf {

void RoundedRectParser::ParseFromNode(const XamlNode& node, Object& reflection_object) {

    auto& rounded_rect = dynamic_cast<RoundedRect&>(reflection_object);
    XamlNodeParseHelper helper(node, rounded_rect.GetType());
    
    auto x_radius = helper.GetFloatProperty(L"XRadius");
    if (x_radius) {
        rounded_rect.x_radius = *x_radius;
    }

    auto y_radius = helper.GetFloatProperty(L"YRadius");
    if (y_radius) {
        rounded_rect.y_radius = *y_radius;
    }

    auto rect = helper.GetObjectProperty<Rect>(L"Rect");
    if (rect) {
        rounded_rect.rect = *rect;
    }
}

}