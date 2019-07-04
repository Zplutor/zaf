#include <zaf/parsing/parsers/clickable_control_parser.h>

namespace zaf {

void ClickableControlParser::ParseFromNode(
    const std::shared_ptr<XamlNode>& node,
    ReflectionObject& reflection_object) {

    __super::ParseFromNode(node, reflection_object);
}

}