#pragma once

#include <zaf/parsing/parsers/list_control_parser.h>

namespace zaf {

class ListBoxParser : public ListControlParser {
public:
    void ParseFromNode(const std::shared_ptr<XamlNode>& node, ReflectionObject& reflection_object) override {

    }
};

}