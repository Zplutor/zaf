#pragma once

namespace zaf {

class ReflectionObject;
class XamlNode;

class Parser {
public:
    Parser() = default;
    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
    virtual ~Parser() = default;

    virtual void ParseFromAttribute(
        const std::wstring& attribute_value, 
        ReflectionObject& reflection_object) { }

    virtual void ParseFromNode(
        const std::shared_ptr<XamlNode>& node, 
        ReflectionObject& reflection_object) = 0;
};

}