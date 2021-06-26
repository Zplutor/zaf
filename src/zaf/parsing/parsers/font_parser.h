#pragma once

#include <optional>
#include <zaf/parsing/object_parser.h>

namespace zaf {

class FontParser : public ObjectParser {
public:
    static std::optional<int> ParseFontWeight(const std::wstring& value);

public:
    void ParseFromNode(const XamlNode& node, Object& reflection_object) override;
};

}