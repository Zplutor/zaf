#pragma once

#include <zaf/parsing/parsers/scrollable_control_parser.h>

namespace zaf {

class ListControlParser : public ScrollableControlParser {
public:
    void ParseFromNode(const std::shared_ptr<XamlNode>& node, ReflectionObject& reflection_object) override {

    }
};

}