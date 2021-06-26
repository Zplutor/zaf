#pragma once

#include <zaf/parsing/parser.h>

namespace zaf {

class ColorParser : public Parser {
public:
    void ParseFromAttribute(
        const std::wstring& attribute, 
        Object& reflection_object) override;

    void ParseFromNode(const XamlNode& node, Object& reflection_object) override;
};

}