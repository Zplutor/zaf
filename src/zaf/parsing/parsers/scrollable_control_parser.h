#pragma once

#include <zaf/parsing/parsers/control_parser.h>

namespace zaf {

class ScrollableControlParser : public ControlParser {
public:
    void ParseFromNode(const XamlNode& node, ReflectionObject& reflection_object) override;
};

}