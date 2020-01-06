#include <zaf/parsing/parsers/check_box_parser.h>

namespace zaf {

void CheckBoxParser::ParseFromNode(const XamlNode& node, ReflectionObject& reflection_object) {
    __super::ParseFromNode(node, reflection_object);
}

}