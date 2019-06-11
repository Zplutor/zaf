#pragma once

#include <zaf/parsing/parsers/control_parser.h>

namespace zaf {

class ScrollBarParser : public ControlParser {
public:
    void Parse(XamlReader& reader, ReflectionObject& reflection_object) override {

    }
};

}