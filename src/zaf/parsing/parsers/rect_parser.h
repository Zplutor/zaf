#pragma once

#include <zaf/parsing/parser.h>

namespace zaf {

class RectParser : public Parser {
public:
    void Parse(XamlReader& reader, ReflectionObject& reflection_object) override;
};

}