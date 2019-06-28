#pragma once

#include <zaf/parsing/parser.h>

namespace zaf {

class ColorParser : public Parser {
public:
    void ParseFromAttribute(
        const std::wstring& attribute, 
        ReflectionObject& reflection_object) override;

    void ParseFromNode(
        const std::shared_ptr<XamlNode>& node, 
        ReflectionObject& reflection_object) override;
};

}