#pragma once

#include <zaf/parsing/parsers/textual_control_parser.h>

namespace zaf {

class LabelParser : public TextualControlParser {
public:
    void Parse(XamlReader& reader, ReflectionObject& reflection_object) override {

    }
};

}