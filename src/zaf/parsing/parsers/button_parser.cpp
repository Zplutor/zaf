#include <zaf/parsing/parsers/button_parser.h>
#include <zaf/control/button.h>

namespace zaf {

void ButtonParser::ParseFromNode(const XamlNode& node, ReflectionObject& reflection_object) {

    __super::ParseFromNode(node, reflection_object);
}

}