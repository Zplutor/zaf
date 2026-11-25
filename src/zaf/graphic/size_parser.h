#pragma once

#include <zaf/core/object/parsing/object_parser.h>

namespace zaf {

class SizeParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, Object& object) override;
};

}
