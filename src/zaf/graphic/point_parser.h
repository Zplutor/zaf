#pragma once

#include <zaf/object/parsing/object_parser.h>

namespace zaf {

class PointParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, Object& object) override;
};

}
