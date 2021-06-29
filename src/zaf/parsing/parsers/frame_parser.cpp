#include <zaf/parsing/parsers/frame_parser.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/graphic/frame.h>
#include <zaf/parsing/parsers/internal/utility.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/parsing/xaml_node_parse_helper.h>
#include <zaf/object/object_type.h>

namespace zaf {
namespace {

Frame ParseFrameValue(const std::wstring& value) {

    float float_value{};
    if (TryToNumeric<float>(value, float_value)) {
        return Frame{ float_value };
    }

    Frame frame;
    internal::ParseAttributeToQuaterFloats(
        value,
        frame.left,
        frame.top,
        frame.right,
        frame.bottom);

    return frame;
}

}

void FrameParser::ParseFromAttribute(
    const std::wstring& attribute_value,
    Object& reflection_object) {

    auto& frame = dynamic_cast<Frame&>(reflection_object);
    frame = ParseFrameValue(attribute_value);
}


void FrameParser::ParseFromNode(const XamlNode& node, Object& reflection_object) {

    auto& frame = dynamic_cast<Frame&>(reflection_object);
    XamlNodeParseHelper helper(node, frame.GetType());

    auto left = helper.GetFloatProperty(L"Left");
    if (left) {
        frame.left = *left;
    }

    auto top = helper.GetFloatProperty(L"Top");
    if (top) {
        frame.top = *top;
    }

    auto right = helper.GetFloatProperty(L"Right");
    if (right) {
        frame.right = *right;
    }

    auto bottom = helper.GetFloatProperty(L"Bottom");
    if (bottom) {
        frame.bottom = *bottom;
    }

    auto content_string = helper.GetContentString();
    if (content_string) {
        frame = ParseFrameValue(*content_string);
    }
}

}