#include <zaf/parsing/parsers/point_parser.h>
#include <zaf/graphic/point.h>
#include <zaf/parsing/parsers/internal/utility.h>
#include <zaf/parsing/utility.h>
#include <zaf/parsing/xaml_node.h>

namespace zaf {

void PointParser::ParseFromAttribute(
    const std::wstring& attribute_value, 
    ReflectionObject& reflection_object) {

    auto& point = dynamic_cast<Point&>(reflection_object);
    internal::ParseAttributeToDoubleFloats(attribute_value, point.x, point.y);
}


void PointParser::ParseFromNode(
    const std::shared_ptr<XamlNode>& node,
    ReflectionObject& reflection_object) {

    Point& point = dynamic_cast<Point&>(reflection_object);

    ParseNodeAttributeToFloat(node, L"x", point.x);
    ParseNodeAttributeToFloat(node, L"y", point.y);

    ParseNodePropertyNodeToFloat(node, L"Point.X", point.x);
    ParseNodePropertyNodeToFloat(node, L"Point.Y", point.y);
}


}