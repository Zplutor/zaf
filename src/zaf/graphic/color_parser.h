#pragma once

#include <zaf/dynamic/parsing/object_parser.h>

namespace zaf {

class ColorParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute, dynamic::Object& object) override;
    void ParseFromNode(const XamlNode& node, dynamic::Object& object) override;
};

}