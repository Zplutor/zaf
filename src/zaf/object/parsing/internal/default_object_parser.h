#pragma once

#include <zaf/object/parsing/object_parser.h>

namespace zaf::internal {

class DefaultObjectParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, Object& object) override;
    void ParseFromNode(const XamlNode& node, Object& object) override;
};

}