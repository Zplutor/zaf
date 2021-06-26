#pragma once

#include <zaf/parsing/parsers/clickable_control_parser.h>

namespace zaf {

class ComboBoxParser : public ClickableControlParser {
public:
    void ParseFromNode(const XamlNode& node, Object& reflection_object) override;
};

}