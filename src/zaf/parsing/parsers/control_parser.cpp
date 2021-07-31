#include <zaf/parsing/parsers/control_parser.h>
#include <zaf/application.h>
#include <zaf/base/error/system_error.h>
#include <zaf/control/control.h>
#include <zaf/parsing/parsers/internal/utility.h>
#include <zaf/object/parsing/xaml_node_parse_helper.h>

namespace zaf {
namespace {

void ParseProperties(const XamlNode& node, Control& control) {

    XamlNodeParseHelper helper(node, control.GetType());

    auto tab_index = helper.GetFloatProperty(L"TabIndex");
    if (tab_index) {
        control.SetTabIndex(static_cast<std::size_t>(*tab_index));
    }
}


void ParseChildren(const XamlNode& node, Control& control) {

    for (const auto& each_node : node.GetContentNodes()) {

        auto child_control = internal::CreateObjectFromNode<Control>(each_node);
        if (!child_control) {
            ZAF_THROW_SYSTEM_ERROR(ERROR_INVALID_NAME);
        }

        control.AddChild(child_control);
    }
}

}

void ControlParser::ParseFromNode(const XamlNode& node, Object& object) {

    auto& control = dynamic_cast<Control&>(object);
    auto update_guard = control.BeginUpdate();

    ParseProperties(node, control);
    ParseChildren(node, control);
}

}