#pragma once

#include <zaf/parsing/object_parser.h>

namespace zaf {

class RectParser : public ObjectParser {
public:
    void ParseFromNode(const XamlNode& node, Object& reflection_object) override;
};

}