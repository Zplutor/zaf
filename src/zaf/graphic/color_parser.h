#pragma once

#include <zaf/dynamic/parsing/object_parser.h>

namespace zaf {

class ColorParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute, Object& object) override;
    void ParseFromNode(const XamlNode& node, Object& object) override;
};

}