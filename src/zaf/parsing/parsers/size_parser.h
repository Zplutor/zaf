#pragma once

#include <zaf/object/parsing/object_parser.h>

namespace zaf {

class SizeParser : public ObjectParser {
public:
    void ParseFromAttribute(
        const std::wstring& attribute_value,
        Object& reflection_object) override;

    void ParseFromNode(const XamlNode& node, Object& reflection_object) override;
};

}