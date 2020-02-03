#include <zaf/parsing/parsers/frame_parser.h>
#include <zaf/graphic/frame.h>
#include <zaf/parsing/parsers/internal/utility.h>
#include <zaf/parsing/xaml_node.h>
#include <zaf/parsing/xaml_node_parse_helper.h>
#include <zaf/reflection/reflection_type.h>

namespace zaf {

void FrameParser::ParseFromAttribute(
    const std::wstring& attribute_value,
    ReflectionObject& reflection_object) {

    auto& frame = dynamic_cast<Frame&>(reflection_object);

    internal::ParseAttributeToQuaterFloats(
        attribute_value, 
        frame.left, 
        frame.top,
        frame.right,
        frame.bottom);
}


void FrameParser::ParseFromNode(const XamlNode& node, ReflectionObject& reflection_object) {

    auto& frame = dynamic_cast<Frame&>(reflection_object);
    XamlNodeParseHelper helper(node, frame.GetType());

    auto left = helper.GetFloat(L"Left");
    if (left) {
        frame.left = *left;
    }

    auto top = helper.GetFloat(L"Top");
    if (top) {
        frame.top = *top;
    }

    auto right = helper.GetFloat(L"Right");
    if (right) {
        frame.right = *right;
    }

    auto bottom = helper.GetFloat(L"Bottom");
    if (bottom) {
        frame.bottom = *bottom;
    }

    auto content_string = helper.GetContentString();
    if (content_string) {
        internal::ParseAttributeToQuaterFloats(
            *content_string,
            frame.left,
            frame.top,
            frame.right,
            frame.bottom);
    }
}

}