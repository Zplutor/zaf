#pragma once

#include <zaf/parsing/parsers/control_parser.h>

namespace zaf {

class SplitControlParser : public ControlParser {
public:
    void ParseFromNode(const std::shared_ptr<XamlNode>& node, ReflectionObject& reflection_object) override {

    }
};

}