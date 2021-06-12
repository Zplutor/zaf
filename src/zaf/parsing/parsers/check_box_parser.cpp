#include <zaf/parsing/parsers/check_box_parser.h>
#include <zaf/control/check_box.h>
#include <zaf/parsing/xaml_node_parse_helper.h>

namespace zaf {
namespace {

std::optional<CheckState> ParseCheckState(const std::wstring& string_value) {
    if (string_value == L"Unchecked") {
        return CheckState::Unchecked;
    }
    if (string_value == L"Checked") {
        return CheckState::Checked;
    }
    if (string_value == L"Indeterminate") {
        return CheckState::Indeterminate;
    }
    return {};
}

}

void CheckBoxParser::ParseFromNode(const XamlNode& node, ReflectionObject& reflection_object) {

    auto& check_box = dynamic_cast<CheckBox&>(reflection_object);
    auto update_guard = check_box.BeginUpdate();

    __super::ParseFromNode(node, reflection_object);

    XamlNodeParseHelper helper(node, reflection_object.GetType());

    auto can_auto_change_check_state = helper.GetBoolProperty(L"CanAutoChangeCheckState");
    if (can_auto_change_check_state) {
        check_box.SetCanAutoChangeCheckState(*can_auto_change_check_state);
    }

    auto can_be_indeterminate = helper.GetBoolProperty(L"CanBeIndeterminate");
    if (can_be_indeterminate) {
        check_box.SetCanBeIndeterminate(*can_be_indeterminate);
    }

    auto is_checked = helper.GetBoolProperty(L"IsChecked");
    if (is_checked) {
        check_box.SetIsChecked(*is_checked);
    }

    auto check_state_string = helper.GetStringProperty(L"CheckState");
    if (check_state_string) {
        auto check_state = ParseCheckState(*check_state_string);
        if (check_state) {
            check_box.SetCheckState(*check_state);
        }
    }
}

}