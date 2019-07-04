#include <zaf/parsing/parsers/window_parser.h>
#include <zaf/application.h>
#include <zaf/parsing/parsers/internal/utility.h>
#include <zaf/parsing/utility.h>
#include <zaf/reflection/reflection_manager.h>
#include <zaf/window/window.h>

namespace zaf {
namespace {

std::optional<Window::Style> ParseStyle(const std::wstring& value) {
    if (value == L"Overlapped") {
        return Window::Style::Overlapped;
    }
    else if (value == L"Popup") {
        return Window::Style::Popup;
    }
    return {};
}


std::optional<Window::InitialRectStyle> ParseInitialRectStyle(const std::wstring& value) {
    if (value == L"CenterInScreen") {
        return Window::InitialRectStyle::CenterInScreen;
    }
    else if (value == L"CenterInOwner") {
        return Window::InitialRectStyle::CenterInOwner;
    }
    else if (value == L"Custom") {
        return Window::InitialRectStyle::Custom;
    }
    return {};
}


std::optional<Window::BorderStyle> ParseBorderStyle(const std::wstring& value) {
    if (value == L"None") {
        return Window::BorderStyle::None;
    }
    else if (value == L"Normal") {
        return Window::BorderStyle::Normal;
    }
    return {};
}


std::shared_ptr<Control> ParseRootControl(const std::shared_ptr<XamlNode>& node) {

    auto parse_property_node = [node]() -> std::shared_ptr<Control> {

        auto property_node = node->GetPropertyNode(L"Window.RootControl");
        if (property_node == nullptr) {
            return {};
        }

        const auto& content_nodes = property_node->GetContentNodes();
        if (content_nodes.size() != 1) {
            return {};
        }
       
        return internal::CreateObjectFromNode<Control>(content_nodes.front());
    };

    auto root_control = parse_property_node();
    if (root_control != nullptr) {
        return root_control;
    }

    auto attribute = node->GetAttribute(L"rootControl");
    if (! attribute) {
        return {};
    }

    return internal::CreateObjectFromTypeName<Control>(*attribute);
}


void ParseProperties(const std::shared_ptr<XamlNode>& node, Window& window) {

    auto style_string = ParseNodeChildToString(node, L"style", L"Window.Style");
    if (style_string) {
        auto style = ParseStyle(*style_string);
        if (style) {
            window.SetStyle(*style);
        }
    }

    auto initial_rect_style_string = ParseNodeChildToString(
        node,
        L"initialRectStyle",
        L"Window.InitialRectStyle");
    if (initial_rect_style_string) {
        auto initial_rect_style = ParseInitialRectStyle(*initial_rect_style_string);
        if (initial_rect_style) {
            window.SetInitialRectStyle(*initial_rect_style);
        }
    }

    auto border_style_string = ParseNodeChildToString(node, L"borderStyle", L"Window.BorderStyle");
    if (border_style_string) {
        auto border_style = ParseBorderStyle(*border_style_string);
        if (border_style) {
            window.SetBorderStyle(*border_style);
        }
    }

    auto title = ParseNodeChildToString(node, L"title", L"Window.Title");
    if (title) {
        window.SetTitle(*title);
    }

    auto width = ParseNodeChildToFloat(node, L"width", L"Window.Width");
    if (width) {
        window.SetWidth(*width);
    }

    auto height = ParseNodeChildToFloat(node, L"height", L"Window.Height");
    if (height) {
        window.SetHeight(*height);
    }

    auto size = ParseNodeChildToObject<Size>(node, L"size", L"Window.Size");
    if (size) {
        window.SetSize(*size);
    }

    auto rect = ParseNodePropertyNodeToObject<Rect>(node, L"Window.Rect");
    if (rect) {
        window.SetRect(*rect);
    }

    auto minimum_width = ParseNodeChildToFloat(node, L"minimumWidth", L"Window.MinimumWidth");
    if (minimum_width) {
        window.SetMinimumWidth(*minimum_width);
    }

    auto minimum_height = ParseNodeChildToFloat(node, L"minimumHeight", L"Window.MinimumHeight");
    if (minimum_height) {
        window.SetMinimumHeight(*minimum_height);
    }

    auto minimum_size = ParseNodeChildToObject<Size>(node, L"minimumSize", L"Window.MinimumSize");
    if (minimum_size) {
        window.SetMinimumSize(*minimum_size);
    }

    auto maximum_width = ParseNodeChildToFloat(node, L"maximumWidth", L"Window.MaximumWidth");
    if (maximum_width) {
        window.SetMaximumWidth(*maximum_width);
    }

    auto maximum_height = ParseNodeChildToFloat(node, L"maximumHeight", L"Window.MaximumHeight");
    if (maximum_height) {
        window.SetMaximumHeight(*maximum_height);
    }

    auto maximum_size = ParseNodeChildToObject<Size>(node, L"maximumSize", L"Window.MaximumSize");
    if (maximum_size) {
        window.SetMaximumSize(*maximum_size);
    }

    auto can_minimize = ParseNodeChildToBool(node, L"canMinimize", L"Window.CanMinimize");
    if (can_minimize) {
        window.SetCanMinimize(*can_minimize);
    }

    auto can_maximize = ParseNodeChildToBool(node, L"canMaximize", L"Window.CanMaximize");
    if (can_maximize) {
        window.SetCanMaximize(*can_maximize);
    }

    auto is_tool_window = ParseNodeChildToBool(node, L"isToolWindow", L"Window.IsToolWindow");
    if (is_tool_window) {
        window.SetIsToolWindow(*is_tool_window);
    }

    auto is_sizable = ParseNodeChildToBool(node, L"isSizable", L"Window.IsSizable");
    if (is_sizable) {
        window.SetIsSizable(*is_sizable);
    }

    auto has_system_menu = ParseNodeChildToBool(node, L"hasSystemMenu", L"Window.HasSystemMenu");
    if (has_system_menu) {
        window.SetHasSystemMenu(*has_system_menu);
    }

    auto root_control = ParseRootControl(node);
    if (root_control != nullptr) {
        window.SetRootControl(root_control);
    }
}


void ParseControls(const std::shared_ptr<XamlNode>& node, Window& window) {

    std::vector<std::shared_ptr<Control>> controls;

    for (const auto& each_node : node->GetContentNodes()) {

        auto control = internal::CreateObjectFromNode<Control>(each_node);
        if (control == nullptr) {
            continue;
        }

        controls.push_back(control);
    }

    if (! controls.empty()) {
        window.GetRootControl()->AddChildren(controls);
    }
}

}

void WindowParser::ParseFromNode(
    const std::shared_ptr<XamlNode>& node,
    ReflectionObject& reflection_object) {

    auto& window = dynamic_cast<Window&>(reflection_object);

    ParseProperties(node, window);
    ParseControls(node, window);
}

}