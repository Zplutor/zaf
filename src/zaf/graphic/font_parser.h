#pragma once

#include <zaf/dynamic/parsing/object_parser.h>

namespace zaf {

class FontParser : public ObjectParser {
public:
    void ParseFromNode(const XamlNode& node, Object& object) override;
};

}