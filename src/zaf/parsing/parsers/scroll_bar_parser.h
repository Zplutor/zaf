#pragma once

#include <zaf/parsing/parsers/control_parser.h>

namespace zaf {

class ScrollBarParser : public ControlParser {
public:
    void ParseFromNode(const std::shared_ptr<XamlNode>& node, ReflectionObject& reflection_object) override {

    }
};

}