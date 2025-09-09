#pragma once

#include <zaf/dynamic/parsing/object_parser.h>

namespace zaf {

class Window;

class WindowParser : public ObjectParser {
public:
    void ParseFromNode(const XamlNode& node, dynamic::Object& reflection_object) override;

private:
    void ParseControls(const XamlNode& node, Window& window);
};

}
