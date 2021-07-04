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
    
    virtual void ParseFromAttribute(const std::wstring& attribute_value, Object& object);
    virtual void ParseFromNode(const XamlNode& node, Object& object);
};

}