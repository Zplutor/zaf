#pragma once

#include <zaf/parsing/parser.h>

namespace zaf {

class SizeParser : public Parser {
public:
    void ParseFromAttribute(
        const std::wstring& attribute_value,
        ReflectionObject& reflection_object) override;

    void ParseFromNode(const XamlNode& node, ReflectionObject& reflection_object) override;
};

}