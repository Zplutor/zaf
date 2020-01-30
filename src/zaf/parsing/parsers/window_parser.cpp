#include <zaf/parsing/parsers/window_parser.h>
#include <zaf/application.h>
#include <zaf/base/string/split.h>
#include <zaf/parsing/parsers/internal/utility.h>
#include <zaf/parsing/xaml_node_parse_helper.h>
#include <zaf/reflection/creation.h>
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


Window::ActivateOption ParseActivateOption(const std::wstring& value) {

    auto activate_option = Window::ActivateOption::None;

    auto splitted_values = SplitIf(value, std::iswspace);
    for (const auto& each_value : splitted_values) {

        if (each_value == L"NoActivate") {
            activate_option |= Window::ActivateOption::NoActivate;
        }
        else if (each_value == L"DiscardMouseMessage") {
            activate_option |= Window::ActivateOption::DiscardMouseMessage;
        }
    }

    return activate_option;
}


std::shared_ptr<Control> ParseRootControl(const XamlNode& node) {

    auto parse_property_node = [&node]() -> std::shared_ptr<Control> {

        auto property_node = node.GetPropertyNode(L"Window.RootControl");
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

    auto attribute = node.GetAttribute(L"RootControl");
    if (! attribute) {
        return {};
    }

    return CreateObjectByName<Control>(*attribute);
}


void ParseProperties(const XamlNode& node, Window& window) {

    XamlNodeParseHelper helper(node, window.GetType());

    auto style_string = helper.GetString(L"Style");
    if (style_string) {
        auto style = ParseStyle(*style_string);
        if (style) {
            window.SetStyle(*style);
        }
    }

    auto initial_rect_style_string = helper.GetString(L"InitialRectStyle");
    if (initial_rect_style_string) {
        auto initial_rect_style = ParseInitialRectStyle(*initial_rect_style_string);
        if (initial_rect_style) {
            window.SetInitialRectStyle(*initial_rect_style);
        }
    }

    auto border_style_string = helper.GetString(L"BorderStyle");
    if (border_style_string) {
        auto border_style = ParseBorderStyle(*border_style_string);
        if (border_style) {
            window.SetBorderStyle(*border_style);
        }
    }

    auto activate_option_string = helper.GetString(L"ActivateOption");
    if (activate_option_string) {
        window.SetActivateOption(ParseActivateOption(*activate_option_string));
    }

    auto title = helper.GetString(L"Title");
    if (title) {
        window.SetTitle(*title);
    }

    auto width = helper.GetFloat(L"Width");
    if (width) {
        window.SetWidth(*width);
    }

    auto height = helper.GetFloat(L"Height");
    if (height) {
        window.SetHeight(*height);
    }

    auto size = helper.GetObjectAsPointer<Size>(L"Size");
    if (size) {
        window.SetSize(*size);
    }

    auto rect = helper.GetObjectAsPointer<Rect>(L"Rect");
    if (rect) {
        window.SetRect(*rect);
    }

    auto minimum_width = helper.GetFloat(L"MinimumWidth");
    if (minimum_width) {
        window.SetMinimumWidth(*minimum_width);
    }

    auto minimum_height = helper.GetFloat(L"MinimumHeight");
    if (minimum_height) {
        window.SetMinimumHeight(*minimum_height);
    }

    auto minimum_size = helper.GetObjectAsPointer<Size>(L"MinimumSize");
    if (minimum_size) {
        window.SetMinimumSize(*minimum_size);
    }

    auto maximum_width = helper.GetFloat(L"MaximumWidth");
    if (maximum_width) {
        window.SetMaximumWidth(*maximum_width);
    }

    auto maximum_height = helper.GetFloat(L"MaximumHeight");
    if (maximum_height) {
        window.SetMaximumHeight(*maximum_height);
    }

    auto maximum_size = helper.GetObjectAsPointer<Size>(L"MaximumSize");
    if (maximum_size) {
        window.SetMaximumSize(*maximum_size);
    }

    auto can_minimize = helper.GetBool(L"CanMinimize");
    if (can_minimize) {
        window.SetCanMinimize(*can_minimize);
    }

    auto can_maximize = helper.GetBool(L"CanMaximize");
    if (can_maximize) {
        window.SetCanMaximize(*can_maximize);
    }

    auto is_tool_window = helper.GetBool(L"IsToolWindow");
    if (is_tool_window) {
        window.SetIsToolWindow(*is_tool_window);
    }

    auto is_sizable = helper.GetBool(L"IsSizable");
    if (is_sizable) {
        window.SetIsSizable(*is_sizable);
    }

    auto has_system_menu = helper.GetBool(L"HasSystemMenu");
    if (has_system_menu) {
        window.SetHasSystemMenu(*has_system_menu);
    }

    auto root_control = ParseRootControl(node);
    if (root_control != nullptr) {
        window.SetRootControl(root_control);
    }
}


void ParseControls(const XamlNode& node, Window& window) {

    std::vector<std::shared_ptr<Control>> controls;

    for (const auto& each_node : node.GetContentNodes()) {

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

void WindowParser::ParseFromNode(const XamlNode& node, ReflectionObject& reflection_object) {

    auto& window = dynamic_cast<Window&>(reflection_object);

    ParseProperties(node, window);
    ParseControls(node, window);
}

}