#pragma once

#include <zaf/dynamic/parsing/object_parser.h>

namespace zaf {

class URIImageParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, dynamic::Object& object) override;
    void ParseFromNode(const XamlNode& node, dynamic::Object& object) override;
};

}