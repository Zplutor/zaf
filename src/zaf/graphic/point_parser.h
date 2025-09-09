#pragma once

#include <zaf/dynamic/parsing/object_parser.h>

namespace zaf {

class PointParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, dynamic::Object& object) override;
};

}
