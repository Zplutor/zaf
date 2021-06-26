#include <zaf/parsing/parsers/linear_layouter_parser.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/parsing/xaml_node_parse_helper.h>

namespace zaf {
namespace {

std::optional<LayoutDirection> ParseLayoutDirection(const std::wstring& value) {

    if (value == L"LeftToRight") {
        return LayoutDirection::LeftToRight;
    }
    if (value == L"TopToBottom") {
        return LayoutDirection::TopToBottom;
    }
    return {};
}


std::optional<ControlAlignment> ParseControlAlignment(const std::wstring& value) {

    if (value == L"Leading") {
        return ControlAlignment::Leading;
    }
    if (value == L"Center") {
        return ControlAlignment::Center;
    }
    if (value == L"Tailing") {
        return ControlAlignment::Tailing;
    }
    return {};
}


std::optional<AxisAlignment> ParseAxisAlignment(const std::wstring& value) {

    if (value == L"Near") {
        return AxisAlignment::Near;
    }
    if (value == L"Center") {
        return AxisAlignment::Center;
    }
    if (value == L"Far") {
        return AxisAlignment::Far;
    }
    return {};
}

}

void LinearLayouterParser::ParseFromNode(
    const XamlNode& node,
    Object& reflection_object) {

    auto& layouter = dynamic_cast<LinearLayouter&>(reflection_object);
    XamlNodeParseHelper helper(node, layouter.GetType());

    auto direction_string = helper.GetStringProperty(L"Direction");
    if (direction_string) {
        auto direction = ParseLayoutDirection(*direction_string);
        if (direction) {
            layouter.SetDirection(*direction);
        }
    }

    auto control_alignment_string = helper.GetStringProperty(L"ControlAlignment");
    if (control_alignment_string) {
        auto control_alignment = ParseControlAlignment(*control_alignment_string);
        if (control_alignment) {
            layouter.SetControlAlignment(*control_alignment);
        }
    }

    auto axis_alignment_string = helper.GetStringProperty(L"AxisAlignment");
    if (axis_alignment_string) {
        auto axis_alignment = ParseAxisAlignment(*axis_alignment_string);
        if (axis_alignment) {
            layouter.SetAxisAlignment(*axis_alignment);
        }
    }
}

}