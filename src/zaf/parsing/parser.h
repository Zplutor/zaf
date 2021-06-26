#pragma once

namespace zaf {

class Object;
class XamlNode;

class Parser {
public:
    Parser() = default;
    virtual ~Parser() = default;

    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;

    virtual void ParseFromAttribute(
        const std::wstring& attribute_value, 
        Object& reflection_object) { }

    virtual void ParseFromNode(
        const XamlNode& node,
        Object& reflection_object) = 0;
};

}