#pragma once

#include <zaf/parsing/parser.h>

namespace zaf {

class RectParser : public Parser {
public:
    void ParseFromNode(const XamlNode& node, Object& reflection_object) override;
};

}