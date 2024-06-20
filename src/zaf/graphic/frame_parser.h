#pragma once

#include <zaf/object/parsing/object_parser.h>

namespace zaf {

class FrameParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& value, Object& object) override;
    void ParseFromNode(const XamlNode& node, Object& object) override;

private:
    static void Parse(const std::wstring& value, Object& object);
};

}