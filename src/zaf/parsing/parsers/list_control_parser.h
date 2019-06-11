#pragma once

#include <zaf/parsing/parsers/scrollable_control_parser.h>

namespace zaf {

class ListControlParser : public ScrollableControlParser {
public:
    void Parse(XamlReader& reader, ReflectionObject& reflection_object) override {

    }
};

}