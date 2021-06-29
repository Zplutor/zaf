#pragma once

#include <zaf/object/parsing/object_parser.h>

namespace zaf {

class TextTrimmingParser : public ObjectParser {
public:
    void ParseFromNode(const XamlNode& node, Object& reflection_object) override {

    }
};

}