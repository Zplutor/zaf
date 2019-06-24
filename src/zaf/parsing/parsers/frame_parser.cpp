#include <zaf/parsing/parsers/frame_parser.h>
#include <zaf/graphic/frame.h>
#include <zaf/parsing/parsers/internal/utility.h>
#include <zaf/parsing/utility.h>
#include <zaf/parsing/xaml_node.h>
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


void FrameParser::ParseFromNode(
    const std::shared_ptr<XamlNode>& node,
    ReflectionObject& reflection_object) {

    auto& frame = dynamic_cast<Frame&>(reflection_object);

    ParseNodeAttributeToFloat(node, L"left", frame.left);
    ParseNodeAttributeToFloat(node, L"top", frame.top);
    ParseNodeAttributeToFloat(node, L"right", frame.right);
    ParseNodeAttributeToFloat(node, L"bottom", frame.bottom);

    ParseNodePropertyNodeToFloat(node, L"Frame.Left", frame.left);
    ParseNodePropertyNodeToFloat(node, L"Frame.Top", frame.top);
    ParseNodePropertyNodeToFloat(node, L"Frame.Right", frame.right);
    ParseNodePropertyNodeToFloat(node, L"Frame.Bottom", frame.bottom);
}

}