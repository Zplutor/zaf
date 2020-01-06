#include <zaf/parsing/parsers/textual_control_parser.h>
#include <zaf/control/textual_control.h>
#include <zaf/parsing/xaml_node_parse_helper.h>

namespace zaf {
namespace {

std::optional<zaf::TextAlignment> ParseTextAlignment(const std::wstring& value) {

    if (value == L"Leading") {
        return zaf::TextAlignment::Leading;
    }
    if (value == L"Tailing") {
        return zaf::TextAlignment::Tailing;
    }
    if (value == L"Center") {
        return zaf::TextAlignment::Center;
    }
    if (value == L"Justified") {
        return zaf::TextAlignment::Justified;
    }
    return {};
}


std::optional<zaf::ParagraphAlignment> ParseParagraphAlignment(const std::wstring& value) {

    if (value == L"Near") {
        return zaf::ParagraphAlignment::Near;
    }
    if (value == L"Far") {
        return zaf::ParagraphAlignment::Far;
    }
    if (value == L"Center") {
        return zaf::ParagraphAlignment::Center;
    }
    return {};
}

}

void TextualControlParser::ParseFromNode(
    const XamlNode& node, 
    ReflectionObject& reflection_object) {

    auto& textual_control = dynamic_cast<TextualControl&>(reflection_object);
    Control::UpdateGuard update_guard(textual_control);

    __super::ParseFromNode(node, reflection_object);

    XamlNodeParseHelper helper(node, textual_control.GetType());

    auto text = helper.GetString(L"Text");
    if (text) {
        textual_control.SetText(*text);
    }

    auto default_text_color = helper.GetObjectAsPointer<Color>(L"DefaultTextColor");
    if (default_text_color) {
        textual_control.SetDefaultTextColor(*default_text_color);
    }

    auto text_alignment_string = helper.GetString(L"TextAlignment");
    if (text_alignment_string) {
        auto text_alignment = ParseTextAlignment(*text_alignment_string);
        if (text_alignment) {
            textual_control.SetTextAlignment(*text_alignment);
        }
    }

    auto paragraph_alignment_string = helper.GetString(L"ParagraphAlignment");
    if (paragraph_alignment_string) {
        auto paragraph_alignment = ParseParagraphAlignment(*paragraph_alignment_string);
        if (paragraph_alignment) {
            textual_control.SetParagraphAlignment(*paragraph_alignment);
        }
    }
}

}