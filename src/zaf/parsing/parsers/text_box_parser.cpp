#include <zaf/parsing/parsers/text_box_parser.h>
#include <zaf/control/text_box.h>
#include <zaf/parsing/xaml_node_parse_helper.h>

namespace zaf {

void TextBoxParser::ParseFromNode(const XamlNode& node, ReflectionObject& reflection_object) {

    auto& text_box = dynamic_cast<TextBox&>(reflection_object);
    auto update_guard = text_box.BeginUpdate();
    
    __super::ParseFromNode(node, reflection_object);

    XamlNodeParseHelper helper(node, text_box.GetType());

    auto is_multiline = helper.GetBoolProperty(L"IsMultiline");
    if (is_multiline) {
        text_box.SetIsMultiline(*is_multiline);
    }
}

}