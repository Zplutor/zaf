#include <zaf/parsing/parsers/control_parser.h>
#include <zaf/application.h>
#include <zaf/control/control.h>
#include <zaf/parsing/parsers/internal/utility.h>
#include <zaf/parsing/xaml_node_parse_helper.h>

namespace zaf {
namespace {

void ParseProperties(const XamlNode& node, Control& control) {

    XamlNodeParseHelper helper(node, control.GetType());

    auto name = helper.GetString(L"Name");
    if (name) {
        control.SetName(*name);
    }

    auto x = helper.GetFloat(L"X");
    if (x) {
        control.SetX(*x);
    }

    auto y = helper.GetFloat(L"Y");
    if (y) {
        control.SetY(*y);
    }

    auto position = helper.GetObjectAsPointer<Point>(L"Position");
    if (position) {
        control.SetPosition(*position);
    }

    auto width = helper.GetFloat(L"Width");
    if (width) {
        control.SetWidth(*width);
    }

    auto height = helper.GetFloat(L"Height");
    if (height) {
        control.SetHeight(*height);
    }

    auto size = helper.GetObjectAsPointer<Size>(L"Size");
    if (size) {
        control.SetSize(*size);
    }

    auto rect = helper.GetObjectAsPointer<Rect>(L"Rect");
    if (rect) {
        control.SetRect(*rect);
    }

    auto minimum_width = helper.GetFloat(L"MinimumWidth");
    if (minimum_width) {
        control.SetMinimumWidth(*minimum_width);
    }

    auto maximum_width = helper.GetFloat(L"MaximumWidth");
    if (maximum_width) {
        control.SetMaximumWidth(*maximum_width);
    }

    auto minimum_height = helper.GetFloat(L"MinimumHeight");
    if (minimum_height) {
        control.SetMinimumHeight(*minimum_height);
    }

    auto maximum_height = helper.GetFloat(L"MaximumHeight");
    if (maximum_height) {
        control.SetMaximumHeight(*maximum_height);
    }

    auto border = helper.GetObjectAsPointer<Frame>(L"Border");
    if (border) {
        control.SetBorder(*border);
    }

    auto padding = helper.GetObjectAsPointer<Frame>(L"Padding");
    if (padding) {
        control.SetPadding(*padding);
    }

    auto is_visible = helper.GetBool(L"IsVisible");
    if (is_visible) {
        control.SetIsVisible(*is_visible);
    }

    auto is_enabled = helper.GetBool(L"IsEnabled");
    if (is_enabled) {
        control.SetIsEnabled(*is_enabled);
    }

    auto can_focused = helper.GetBool(L"CanFocused");
    if (can_focused) {
        control.SetCanFocused(*can_focused);
    }

    auto can_tab_stop = helper.GetBool(L"CanTabStop");
    if (can_tab_stop) {
        control.SetCanTabStop(*can_tab_stop);
    }

    auto tab_index = helper.GetFloat(L"TabIndex");
    if (tab_index) {
        control.SetTabIndex(static_cast<std::size_t>(*tab_index));
    }

    auto background_color = helper.GetObjectAsPointer<Color>(L"BackgroundColor");
    if (background_color) {
        control.SetBackgroundColor(*background_color);
    }

    auto border_color = helper.GetObjectAsPointer<Color>(L"BorderColor");
    if (border_color) {
        control.SetBorderColor(*border_color);
    }
}


void ParseChildren(const XamlNode& node, Control& control) {

    for (const auto& each_node : node.GetContentNodes()) {

        auto child_control = internal::CreateObjectFromNode<Control>(each_node);
        if (child_control == nullptr) {
            continue;
        }

        control.AddChild(child_control);
    }
}

}

void ControlParser::ParseFromNode(const XamlNode& node, ReflectionObject& reflection_object) {

    auto& control = dynamic_cast<Control&>(reflection_object);
    Control::UpdateGuard update_guard(control);

    ParseProperties(node, control);
    ParseChildren(node, control);
}

}