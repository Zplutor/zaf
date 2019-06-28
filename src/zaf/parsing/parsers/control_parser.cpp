#include <zaf/parsing/parsers/control_parser.h>
#include <zaf/application.h>
#include <zaf/control/control.h>
#include <zaf/parsing/utility.h>
#include <zaf/reflection/reflection_manager.h>
#include <zaf/reflection/reflection_type.h>

namespace zaf {
namespace {

void ParseProperties(const std::shared_ptr<XamlNode>& node, Control& control) {

    auto name = ParseNodeChildToString(node, L"name", L"Control.Name");
    if (name) {
        control.SetName(*name);
    }

    auto x = ParseNodeChildToFloat(node, L"x", L"Control.X");
    if (x) {
        control.SetX(*x);
    }

    auto y = ParseNodeChildToFloat(node, L"y", L"Control.Y");
    if (y) {
        control.SetY(*y);
    }

    auto position = ParseNodeChildToObject<Point>(node, L"position", L"Control.Position");
    if (position) {
        control.SetPosition(*position);
    }

    auto width = ParseNodeChildToFloat(node, L"width", L"Control.Width");
    if (width) {
        control.SetWidth(*width);
    }

    auto height = ParseNodeChildToFloat(node, L"height", L"Control.Height");
    if (height) {
        control.SetHeight(*height);
    }

    auto size = ParseNodeChildToObject<Size>(node, L"size", L"Control.Size");
    if (size) {
        control.SetSize(*size);
    }

    auto rect = ParseNodePropertyNodeToObject<Rect>(node, L"Control.Rect");
    if (rect) {
        control.SetRect(*rect);
    }

    auto minimum_width = ParseNodeChildToFloat(node, L"minimumWidth", L"Control.MinimumWidth");
    if (minimum_width) {
        control.SetMinimumWidth(*minimum_width);
    }

    auto maximum_width = ParseNodeChildToFloat(node, L"maximumWidth", L"Control.MaximumWidth");
    if (maximum_width) {
        control.SetMaximumWidth(*maximum_width);
    }

    auto minimum_height = ParseNodeChildToFloat(node, L"minimumHeight", L"Control.MinimumHeight");
    if (minimum_height) {
        control.SetMinimumHeight(*minimum_height);
    }

    auto maximum_height = ParseNodeChildToFloat(node, L"maximumHeight", L"Control.MaximumHeight");
    if (maximum_height) {
        control.SetMaximumHeight(*maximum_height);
    }

    auto border = ParseNodeChildToObject<Frame>(node, L"border", L"Control.Border");
    if (border) {
        control.SetBorder(*border);
    }

    auto padding = ParseNodeChildToObject<Frame>(node, L"padding", L"Control.Padding");
    if (padding) {
        control.SetPadding(*padding);
    }

    auto is_visible = ParseNodeChildToBool(node, L"isVisible", L"Control.IsVisible");
    if (is_visible) {
        control.SetIsVisible(*is_visible);
    }

    auto is_enabled = ParseNodeChildToBool(node, L"isEnabled", L"Control.IsEnabled");
    if (is_enabled) {
        control.SetIsEnabled(*is_enabled);
    }

    auto can_focused = ParseNodeChildToBool(node, L"canFocused", L"Control.CanFocused");
    if (can_focused) {
        control.SetCanFocused(*can_focused);
    }

    auto can_tab_stop = ParseNodeChildToBool(node, L"canTabStop", L"Control.CanTabStop");
    if (can_tab_stop) {
        control.SetCanTabStop(*can_tab_stop);
    }

    auto tab_index = ParseNodeChildToFloat(node, L"tabIndex", L"Control.TabIndex");
    if (tab_index) {
        control.SetTabIndex(static_cast<std::size_t>(*tab_index));
    }
}


void ParseChildren(const std::shared_ptr<XamlNode>& node, Control& control) {

    for (const auto& each_node : node->GetContentNodes()) {

        const auto& name = each_node->GetValue();
        auto type = GetReflectionManager().GetType(name);
        if (type == nullptr) {
            continue;
        }

        auto object = type->CreateInstance();
        auto child_control = std::dynamic_pointer_cast<Control>(object);
        if (child_control == nullptr) {
            continue;
        }

        type->GetParser()->ParseFromNode(each_node, *object);
        control.AddChild(child_control);
    }
}

}

void ControlParser::ParseFromNode(
    const std::shared_ptr<XamlNode>& node,
    ReflectionObject& reflection_object) {

    auto& control = dynamic_cast<Control&>(reflection_object);
    Control::UpdateGuard update_guard(control);

    ParseProperties(node, control);
    ParseChildren(node, control);
}

}