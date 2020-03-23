#pragma once

#include <zaf/parsing/parser.h>

namespace zaf {

class LinearLayouterParser : public Parser {
public:
    void ParseFromNode(
        const XamlNode& node,
        ReflectionObject& reflection_object) override;
};

}