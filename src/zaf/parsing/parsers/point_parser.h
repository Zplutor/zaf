#pragma once

#include <zaf/parsing/object_parser.h>

namespace zaf {

class Point;

class PointParser : public ObjectParser {
public:
    void ParseFromAttribute(
        const std::wstring& attribute_value, 
        Object& reflection_object) override;

    void ParseFromNode(const XamlNode& node, Object& reflection_object) override;
};

}