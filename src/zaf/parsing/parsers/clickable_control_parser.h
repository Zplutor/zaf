#pragma once

#include <zaf/parsing/parsers/textual_control_parser.h>

namespace zaf {

class ClickableControlParser : public TextualControlParser {
public:
    void ParseFromNode(const XamlNode& node, ReflectionObject& reflection_object) override;
};

}