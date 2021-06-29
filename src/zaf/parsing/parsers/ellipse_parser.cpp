#include <zaf/parsing/parsers/ellipse_parser.h>
#include <zaf/graphic/ellipse.h>
#include <zaf/object/parsing/xaml_node_parse_helper.h>

namespace zaf {

void EllipseParser::ParseFromNode(const XamlNode& node, Object& reflection_object) {

    auto& ellipse = dynamic_cast<Ellipse&>(reflection_object);
    XamlNodeParseHelper helper(node, ellipse.GetType());

    auto position = helper.GetObjectProperty<Point>(L"Position");
    if (position) {
        ellipse.position = *position;
    }

    auto x_radius = helper.GetFloatProperty(L"XRadius");
    if (x_radius) {
        ellipse.x_radius = *x_radius;
    }

    auto y_radius = helper.GetFloatProperty(L"YRadius");
    if (y_radius) {
        ellipse.y_radius = *y_radius;
    }
}

}