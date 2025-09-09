#pragma once

#include <zaf/dynamic/parsing/object_parser.h>

namespace zaf {

class IfParser : public ObjectParser {
public:
    void ParseFromNode(const XamlNode& node, dynamic::Object& object) override;
};

}