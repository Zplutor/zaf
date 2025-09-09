#pragma once

#include <zaf/dynamic/parsing/object_parser.h>

namespace zaf {

class BooleanParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& value, Object& object) override;
    void ParseFromNode(const XamlNode& node, Object& object) override;

private:
    void Parse(const std::wstring& string, Object& object);
};

}