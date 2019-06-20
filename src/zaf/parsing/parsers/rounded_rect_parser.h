#pragma once

#include <zaf/parsing/parser.h>

namespace zaf {

class RoundedRectParser : public Parser {
public:
    void ParseFromNode(const std::shared_ptr<XamlNode>& node, ReflectionObject& reflection_object) override {

    }
};

}