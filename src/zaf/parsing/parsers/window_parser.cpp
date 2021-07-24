#include <zaf/parsing/parsers/window_parser.h>
#include <zaf/application.h>
#include <zaf/base/string/split.h>
#include <zaf/parsing/parsers/internal/utility.h>
#include <zaf/object/parsing/xaml_node_parse_helper.h>
#include <zaf/reflection/creation.h>
#include <zaf/window/window.h>

namespace zaf {
namespace {

ActivateOption ParseActivateOption(const std::wstring& value) {

    auto activate_option = ActivateOption::None;

    auto splitted_values = SplitIf(value, std::iswspace);
    for (const auto& each_value : splitted_values) {

        if (each_value == L"NoActivate") {
            activate_option |= ActivateOption::NoActivate;
        }
        else if (each_value == L"DiscardMouseMessage") {
            activate_option |= ActivateOption::DiscardMouseMessage;
        }
    }

    return activate_option;
}


void ParseProperties(const XamlNode& node, Window& window) {

    XamlNodeParseHelper helper(node, window.GetType());

    auto activate_option_string = helper.GetStringProperty(L"ActivateOption");
    if (activate_option_string) {
        window.SetActivateOption(ParseActivateOption(*activate_option_string));
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
        window.RootControl()->AddChildren(controls);
    }
}

}

void WindowParser::ParseFromNode(const XamlNode& node, Object& reflection_object) {

    auto& window = dynamic_cast<Window&>(reflection_object);

    ParseProperties(node, window);
    ParseControls(node, window);
}

}