#pragma once

#include <zaf/parsing/parsers/list_control_parser.h>

namespace zaf {

class ListBoxParser : public ListControlParser {
public:
    void Parse(XamlReader& reader, ReflectionObject& reflection_object) override {

    }
};

}