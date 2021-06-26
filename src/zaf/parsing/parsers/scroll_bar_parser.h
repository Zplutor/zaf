#pragma once

#include <zaf/parsing/parsers/control_parser.h>

namespace zaf {

class ScrollBarParser : public ControlParser {
public:
    void ParseFromNode(const XamlNode& node, Object& reflection_object) override {
        __super::ParseFromNode(node, reflection_object);
    }
};

}