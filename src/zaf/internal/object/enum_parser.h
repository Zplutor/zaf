#pragma once

#include <zaf/dynamic/enum_type.h>
#include <zaf/dynamic/parsing/object_parser.h>

namespace zaf::internal {

class EnumParser : public ObjectParser {
public:
    explicit EnumParser(const EnumType* type);

    void ParseFromAttribute(const std::wstring& attribute_value, dynamic::Object& object) override;
    void ParseFromNode(const XamlNode& node, dynamic::Object& object) override;

private:
    void Parse(const std::wstring& text, dynamic::Object& object);
    std::shared_ptr<dynamic::Object> ParseValue(const std::wstring& text);
    std::shared_ptr<dynamic::Object> ParseSingleValue(const std::wstring& text);

private:
    const EnumType* enum_type_{};
};

}