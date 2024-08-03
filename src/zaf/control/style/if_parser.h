#pragma once

#include <zaf/object/parsing/object_parser.h>

namespace zaf {

class IfParser : public ObjectParser {
public:
    void ParseFromNode(const XamlNode& node, Object& object) override;
};

}