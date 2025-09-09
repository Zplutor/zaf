#pragma once

#include <zaf/dynamic/parsing/object_parser.h>

namespace zaf {

class FontWeightParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, dynamic::Object& object) override;
    void ParseFromNode(const XamlNode& node, dynamic::Object& object) override;

private:
    void Parse(const std::wstring& string_value, dynamic::Object& object);
    int ParseValue(const std::wstring& value);
};

}