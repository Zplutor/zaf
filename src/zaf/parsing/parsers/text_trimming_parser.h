#pragma once

#include <zaf/parsing/parser.h>

namespace zaf {

class TextTrimmingParser : public Parser {
public:
    void Parse(XamlReader& reader, ReflectionObject& reflection_object) override {

    }
};

}