#include <zaf/parsing/parsers/ellipse_parser.h>
#include <zaf/graphic/ellipse.h>
#include <zaf/parsing/utility.h>
#include <zaf/reflection/reflection_type.h>

namespace zaf {

void EllipseParser::ParseFromNode(
    const std::shared_ptr<XamlNode>& node,
    ReflectionObject& reflection_object) {

    auto& ellipse = dynamic_cast<Ellipse&>(reflection_object);

    ParseNodeAttributeToObject(node, L"position", ellipse.position);
    ParseNodeAttributeToFloat(node, L"xRadius", ellipse.x_radius);
    ParseNodeAttributeToFloat(node, L"yRadius", ellipse.y_radius);

    ParseNodePropertyNodeToObject(node, L"Ellipse.Position", ellipse.position);
    ParseNodePropertyNodeToFloat(node, L"Ellipse.XRadius", ellipse.x_radius);
    ParseNodePropertyNodeToFloat(node, L"Ellipse.YRadius", ellipse.y_radius);
}

}