#include <zaf/parsing/parsers/clickable_control_parser.h>

namespace zaf {

void ClickableControlParser::ParseFromNode(
    const XamlNode& node, 
    Object& reflection_object) {

    __super::ParseFromNode(node, reflection_object);
}

}