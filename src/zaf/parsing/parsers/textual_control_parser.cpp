#include <zaf/parsing/parsers/textual_control_parser.h>
#include <zaf/control/textual_control.h>
#include <zaf/graphic/font/font.h>
#include <zaf/parsing/parsers/font_parser.h>
#include <zaf/object/parsing/xaml_node_parse_helper.h>

namespace zaf {

void TextualControlParser::ParseFromNode(
    const XamlNode& node, 
    Object& reflection_object) {

    auto& textual_control = dynamic_cast<TextualControl&>(reflection_object);
    auto update_guard = textual_control.BeginUpdate();

    XamlNodeParseHelper helper(node, textual_control.GetType());

    auto font = helper.GetObjectProperty<Font>(L"Font");
    if (font) {
        textual_control.SetFont(*font);
    }

    auto font_weight_string = helper.GetStringProperty(L"FontWeight");
    if (font_weight_string) {
        auto weight = FontParser::ParseFontWeight(*font_weight_string);
        if (weight) {
            textual_control.SetFontWeight(*weight);
        }
    }
}

}