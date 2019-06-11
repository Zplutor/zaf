#pragma once

namespace zaf {

class XamlReader;
class ReflectionObject;

class Parser {
public:
    Parser() = default;
    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
    virtual ~Parser() = default;

    virtual void Parse(XamlReader& reader, ReflectionObject& reflection_object) = 0;
};

}