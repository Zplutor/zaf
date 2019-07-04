#include <zaf/parsing/parsers/button_parser.h>
#include <zaf/control/button.h>

namespace zaf {

void ButtonParser::ParseFromNode(
    const std::shared_ptr<XamlNode>& node,
    ReflectionObject& reflection_object) {

    __super::ParseFromNode(node, reflection_object);
}

}