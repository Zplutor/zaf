#pragma once

#include <zaf/parsing/parsers/clickable_control_parser.h>

namespace zaf {

class ButtonParser : public ClickableControlParser {
public:
    void Parse(XamlReader& reader, ReflectionObject& reflection_object) override {

    }
};

}