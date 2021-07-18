#pragma once

#include <optional>
#include <zaf/object/parsing/object_parser.h>

namespace zaf {

class FontParser : public ObjectParser {
public:
    static std::optional<int> ParseFontWeight(const std::wstring& value);

public:
    void ParseFromAttribute(const std::wstring& attribute_value, Object& object) override {

    }

    void ParseFromNode(const XamlNode& node, Object& reflection_object) override;
};

}