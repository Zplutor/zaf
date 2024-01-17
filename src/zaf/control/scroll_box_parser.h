#pragma once

#include <zaf/control/control_parser.h>

namespace zaf {

class ScrollBoxParser : public ControlParser {
public:
    void ParseContentNodes(
        const std::vector<std::shared_ptr<XamlNode>>& nodes,
        Control& control) override;
};

}