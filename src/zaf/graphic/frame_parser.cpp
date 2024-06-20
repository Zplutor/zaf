#include <zaf/graphic/frame_parser.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/graphic/frame.h>
#include <zaf/object/parsing/internal/utility.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/object/parsing/xaml_utility.h>

namespace zaf {

void FrameParser::ParseFromAttribute(const std::wstring& value, Object& object) {
    Parse(value, object);
}


void FrameParser::ParseFromNode(const XamlNode& node, Object& object) {

    __super::ParseFromNode(node, object);

    auto content_string = GetContentStringFromXamlNode(node);
    if (content_string) {
        Parse(*content_string, object);
    }
}


void FrameParser::Parse(const std::wstring& value, Object& object) {

    auto& frame = As<Frame>(object);

    float float_value{};
    if (TryToNumeric<float>(value, float_value)) {
        frame = Frame{ float_value };
        return;
    }

    bool is_succeeded = internal::ParseAttributeToQuaterFloats(
        value,
        frame.left,
        frame.top,
        frame.right,
        frame.bottom);

    if (!is_succeeded) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }
}

}