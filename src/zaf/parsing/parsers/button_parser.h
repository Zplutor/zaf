#pragma once

#include <zaf/parsing/parsers/clickable_control_parser.h>

namespace zaf {

class ButtonParser : public ClickableControlParser {
public:
    void ParseFromNode(
        const std::shared_ptr<XamlNode>& node,
        ReflectionObject& reflection_object) override;
};

}