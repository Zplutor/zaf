#include <zaf/parsing/parsers/textual_control_parser.h>
#include <zaf/control/textual_control.h>
#include <zaf/parsing/utility.h>

namespace zaf {

void TextualControlParser::ParseFromNode(
    const std::shared_ptr<XamlNode>& node,
    ReflectionObject& reflection_object) {

    auto& textual_control = dynamic_cast<TextualControl&>(reflection_object);
    Control::UpdateGuard update_guard(textual_control);

    __super::ParseFromNode(node, reflection_object);

    auto text = ParseNodeChildToString(node, L"text", L"TextualControl.Text");
    if (text) {
        textual_control.SetText(*text);
    }

    auto default_text_color = ParseNodeChildToObject<Color>(
        node,
        L"defaultTextColor",
        L"TextualControl.DefaultTextColor");
    if (default_text_color) {
        textual_control.SetDefaultTextColor(*default_text_color);
    }
}

}