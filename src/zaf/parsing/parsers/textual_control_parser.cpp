#include <zaf/parsing/parsers/textual_control_parser.h>
#include <zaf/control/textual_control.h>
#include <zaf/parsing/xaml_node_parse_helper.h>

namespace zaf {
namespace {

std::optional<TextAlignment> ParseTextAlignment(const std::wstring& value) {

    if (value == L"Leading") {
        return TextAlignment::Leading;
    }
    if (value == L"Tailing") {
        return TextAlignment::Tailing;
    }
    if (value == L"Center") {
        return TextAlignment::Center;
    }
    if (value == L"Justified") {
        return TextAlignment::Justified;
    }
    return {};
}


std::optional<ParagraphAlignment> ParseParagraphAlignment(const std::wstring& value) {

    if (value == L"Near") {
        return ParagraphAlignment::Near;
    }
    if (value == L"Far") {
        return ParagraphAlignment::Far;
    }
    if (value == L"Center") {
        return ParagraphAlignment::Center;
    }
    return {};
}


std::optional<WordWrapping> ParseWordWrapping(const std::wstring& value) {

    if (value == L"Wrap") {
        return WordWrapping::Wrap;
    }
    if (value == L"NoWrap") {
        return WordWrapping::NoWrap;
    }
    if (value == L"EmergencyBreak") {
        return WordWrapping::EmergencyBreak;
    }
    if (value == L"WholeWord") {
        return WordWrapping::WholeWord;
    }
    if (value == L"Character") {
        return WordWrapping::Character;
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

    auto word_wrapping_string = helper.GetString(L"WordWrapping");
    if (word_wrapping_string) {
        auto word_wrapping = ParseWordWrapping(*word_wrapping_string);
        if (word_wrapping) {
            textual_control.SetWordWrapping(*word_wrapping);
        }
    }
}

}