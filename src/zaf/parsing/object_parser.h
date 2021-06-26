#pragma once

namespace zaf {

class Object;
class XamlNode;

class ObjectParser {
public:
    ObjectParser() = default;
    virtual ~ObjectParser() = default;

    ObjectParser(const ObjectParser&) = delete;
    ObjectParser& operator=(const ObjectParser&) = delete;

    virtual void ParseFromAttribute(
        const std::wstring& attribute_value, 
        Object& reflection_object) { }

    virtual void ParseFromNode(
        const XamlNode& node,
        Object& reflection_object) = 0;
};

}