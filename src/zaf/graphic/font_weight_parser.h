#pragma once

#include <zaf/dynamic/parsing/object_parser.h>

namespace zaf {

class FontWeightParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, Object& object) override;
    void ParseFromNode(const XamlNode& node, Object& object) override;

private:
    void Parse(const std::wstring& string_value, Object& object);
    int ParseValue(const std::wstring& value);
};

}