#include <zaf/parsing/parsers/textual_control_parser.h>
#include <zaf/control/textual_control.h>
#include <zaf/parsing/xaml_node_parse_helper.h>

namespace zaf {

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
}

}