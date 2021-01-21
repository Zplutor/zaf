#include <zaf/parsing/parsers/scrollable_control_parser.h>
#include <zaf/control/scrollable_control.h>
#include <zaf/parsing/xaml_node_parse_helper.h>

namespace zaf {

void ScrollableControlParser::ParseFromNode(
    const XamlNode& node,
    ReflectionObject& reflection_object) {

    auto& scrollable_control = dynamic_cast<ScrollableControl&>(reflection_object);
    Control::UpdateGuard update_guard(scrollable_control);

    __super::ParseFromNode(node, scrollable_control);

    XamlNodeParseHelper helper(node, scrollable_control.GetType());

    auto scroll_content_control = helper.GetDynamicObjectProperty<Control>(L"ScrollContentControl");
    if (scroll_content_control) {
        scrollable_control.SetScrollContentControl(scroll_content_control);
    }
}

}