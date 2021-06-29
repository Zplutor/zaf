#include <zaf/parsing/parsers/point_parser.h>
#include <zaf/graphic/point.h>
#include <zaf/parsing/parsers/internal/utility.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/parsing/xaml_node_parse_helper.h>

namespace zaf {

void PointParser::ParseFromAttribute(
    const std::wstring& attribute_value, 
    Object& reflection_object) {

    auto& point = dynamic_cast<Point&>(reflection_object);
    internal::ParseAttributeToDoubleFloats(attribute_value, point.x, point.y);
}


void PointParser::ParseFromNode(const XamlNode& node, Object& reflection_object) {

    Point& point = dynamic_cast<Point&>(reflection_object);
    XamlNodeParseHelper helper(node, point.GetType());

    auto x = helper.GetFloatProperty(L"X");
    if (x) {
        point.x = *x;
    }

    auto y = helper.GetFloatProperty(L"Y");
    if (y) {
        point.y = *y;
    }
}


}