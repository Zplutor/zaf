#pragma once

namespace zaf {

class ReflectionObject;
class XamlNode;

class Parser {
public:
    Parser() = default;
    virtual ~Parser() = default;

    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;

    virtual void ParseFromAttribute(
        const std::wstring& attribute_value, 
        ReflectionObject& reflection_object) { }

    virtual void ParseFromNode(
        const XamlNode& node,
        ReflectionObject& reflection_object) = 0;
};

}