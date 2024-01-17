#include <zaf/control/scrollable_control_parser.h>
#include <zaf/base/as.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/control/scroll_box.h>
#include <zaf/object/parsing/internal/utility.h>

namespace zaf {

void ScrollableControlParser::ParseContentNodes(
    const std::vector<std::shared_ptr<XamlNode>>& nodes,
    Control& control) {

    if (nodes.empty()) {
        return;
    }

    //Multiple content nodes are not allowed.
    if (nodes.size() > 1) {
        ZAF_THROW_ERRC(BasicErrc::InvalidValue);
    }

    auto scroll_content_control = internal::CreateObjectFromNode<Control>(nodes.front());
    if (!scroll_content_control) {
        ZAF_THROW_ERRC(BasicErrc::InvalidValue);
    }

    As<ScrollBox>(control).SetScrollContent(scroll_content_control);
}

}