#include <zaf/control/scroll_box_parser.h>
#include <zaf/base/as.h>
#include <zaf/dynamic/parsing/parse_error.h>
#include <zaf/control/scroll_box.h>
#include <zaf/dynamic/parsing/internal/utility.h>

namespace zaf {

void ScrollBoxParser::ParseContentNodes(
    const std::vector<std::shared_ptr<XamlNode>>& nodes,
    Control& control) {

    if (nodes.empty()) {
        return;
    }

    //Multiple content nodes are not allowed.
    if (nodes.size() > 1) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }

    auto scroll_content_control = internal::CreateObjectFromNode<Control>(nodes.front());
    if (!scroll_content_control) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }

    As<ScrollBox>(control).SetScrollContent(scroll_content_control);
}

}