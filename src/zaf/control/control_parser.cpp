#include <zaf/control/control_parser.h>
#include <zaf/base/as.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/control/control.h>
#include <zaf/object/parsing/xaml_node_parse_helper.h>

namespace zaf {

void ControlParser::ParseFromNode(const XamlNode& node, Object& object) {

    __super::ParseFromNode(node, object);

    auto& control = As<Control>(object);
    auto update_guard = control.BeginUpdate();

    XamlNodeParseHelper helper(node, control.DynamicType());
    auto tab_index = helper.GetFloatProperty(L"TabIndex");
    if (tab_index) {
        control.SetTabIndex(static_cast<std::size_t>(*tab_index));
    }

    ParseStyles(node, control);

    ParseContentNodes(node.GetContentNodes(), control);
}


void ControlParser::ParseContentNodes(
    const std::vector<std::shared_ptr<XamlNode>>& nodes,
    Control& control) {

    for (const auto& each_node : nodes) {

        auto child_control = internal::CreateObjectFromNode<Control>(each_node);
        if (!child_control) {
            throw ParseError{ ZAF_SOURCE_LOCATION() };
        }

        control.AddChild(child_control);
    }
}


void ControlParser::ParseStyles(const XamlNode& node, Control& control) {

    auto style_node = node.FindPropertyNode(L"Styles");
    if (!style_node) {
        return;
    }

    for (const auto& each_node : style_node->GetContentNodes()) {

        auto style = internal::CreateObjectFromNode<Style>(each_node);
        control.Styles().Add(std::move(style));
    }
}

}