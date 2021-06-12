#include <zaf/parsing/parsers/window_parser.h>
#include <zaf/application.h>
#include <zaf/base/string/split.h>
#include <zaf/parsing/parsers/internal/utility.h>
#include <zaf/parsing/xaml_node_parse_helper.h>
#include <zaf/reflection/creation.h>
#include <zaf/window/window.h>

namespace zaf {
namespace {

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


void ParseProperties(const XamlNode& node, Window& window) {

    XamlNodeParseHelper helper(node, window.GetType());

    auto is_popup = helper.GetBoolProperty(L"IsPopup");
    if (is_popup) {
        window.SetIsPopup(*is_popup);
    }

    auto initial_rect_style_string = helper.GetStringProperty(L"InitialRectStyle");
    if (initial_rect_style_string) {
        auto initial_rect_style = ParseInitialRectStyle(*initial_rect_style_string);
        if (initial_rect_style) {
            window.SetInitialRectStyle(*initial_rect_style);
        }
    }

    auto has_border = helper.GetBoolProperty(L"HasBorder");
    if (has_border) {
        window.SetHasBorder(*has_border);
    }

    auto has_title_bar = helper.GetBoolProperty(L"HasTitleBar");
    if (has_title_bar) {
        window.SetHasTitleBar(*has_title_bar);
    }

    auto activate_option_string = helper.GetStringProperty(L"ActivateOption");
    if (activate_option_string) {
        window.SetActivateOption(ParseActivateOption(*activate_option_string));
    }

    auto title = helper.GetStringProperty(L"Title");
    if (title) {
        window.SetTitle(*title);
    }

    auto width = helper.GetFloatProperty(L"Width");
    if (width) {
        window.SetWidth(*width);
    }

    auto height = helper.GetFloatProperty(L"Height");
    if (height) {
        window.SetHeight(*height);
    }

    auto size = helper.GetObjectProperty<Size>(L"Size");
    if (size) {
        window.SetSize(*size);
    }

    auto client_size = helper.GetObjectProperty<Size>(L"ClientSize");
    if (client_size) {
        window.SetClientSize(*client_size);
    }

    auto position = helper.GetObjectProperty<Point>(L"Position");
    if (position) {
        window.SetPosition(*position);
    }

    auto rect = helper.GetObjectProperty<Rect>(L"Rect");
    if (rect) {
        window.SetRect(*rect);
    }

    auto minimum_width = helper.GetFloatProperty(L"MinWidth");
    if (minimum_width) {
        window.SetMinWidth(*minimum_width);
    }

    auto minimum_height = helper.GetFloatProperty(L"MinHeight");
    if (minimum_height) {
        window.SetMinHeight(*minimum_height);
    }

    auto minimum_size = helper.GetObjectProperty<Size>(L"MinSize");
    if (minimum_size) {
        window.SetMinSize(*minimum_size);
    }

    auto maximum_width = helper.GetFloatProperty(L"MaxWidth");
    if (maximum_width) {
        window.SetMaxWidth(*maximum_width);
    }

    auto maximum_height = helper.GetFloatProperty(L"MaxHeight");
    if (maximum_height) {
        window.SetMaxHeight(*maximum_height);
    }

    auto maximum_size = helper.GetObjectProperty<Size>(L"MaxSize");
    if (maximum_size) {
        window.SetMaxSize(*maximum_size);
    }

    auto can_minimize = helper.GetBoolProperty(L"CanMinimize");
    if (can_minimize) {
        window.SetCanMinimize(*can_minimize);
    }

    auto can_maximize = helper.GetBoolProperty(L"CanMaximize");
    if (can_maximize) {
        window.SetCanMaximize(*can_maximize);
    }

    auto is_tool_window = helper.GetBoolProperty(L"IsToolWindow");
    if (is_tool_window) {
        window.SetIsToolWindow(*is_tool_window);
    }

    auto is_sizable = helper.GetBoolProperty(L"IsSizable");
    if (is_sizable) {
        window.SetIsSizable(*is_sizable);
    }

    auto has_system_menu = helper.GetBoolProperty(L"HasSystemMenu");
    if (has_system_menu) {
        window.SetHasSystemMenu(*has_system_menu);
    }

    auto root_control = helper.GetDynamicObjectProperty<Control>(L"RootControl");
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