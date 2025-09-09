#pragma once

#include <zaf/dynamic/parsing/object_parser.h>

namespace zaf {

class FrameParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& value, dynamic::Object& object) override;
    void ParseFromNode(const XamlNode& node, dynamic::Object& object) override;

private:
    static void Parse(const std::wstring& value, dynamic::Object& object);
};

}