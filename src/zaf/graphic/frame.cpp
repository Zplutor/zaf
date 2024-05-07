#include <zaf/graphic/frame.h>
#include <zaf/base/as.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/base/string/to_string.h>
#include <zaf/object/parsing/internal/utility.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_utility.h>
#include <zaf/object/type_definition.h>

namespace zaf {
namespace {

class FrameParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& value, Object& object) override {
        Parse(value, object);
    }

    void ParseFromNode(const XamlNode& node, Object& object) override {

        __super::ParseFromNode(node, object);

        auto content_string = GetContentStringFromXamlNode(node);
        if (content_string) {
            Parse(*content_string, object);
        }
    }

private:
    static void Parse(const std::wstring& value, Object& object) {

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
};

}


ZAF_DEFINE_TYPE(Frame)
ZAF_DEFINE_TYPE_PARSER(FrameParser)
ZAF_DEFINE_TYPE_PROPERTY(Left)
ZAF_DEFINE_TYPE_PROPERTY(Top)
ZAF_DEFINE_TYPE_PROPERTY(Right)
ZAF_DEFINE_TYPE_PROPERTY(Bottom)
ZAF_DEFINE_TYPE_PROPERTY(Width)
ZAF_DEFINE_TYPE_PROPERTY(Height)
ZAF_DEFINE_TYPE_END

ZAF_DEFINE_EQUALITY(Frame)


std::wstring Frame::ToString() const {
    return
        ToWideString(left) + L',' +
        ToWideString(top) + L',' +
        ToWideString(right) + L',' +
        ToWideString(bottom);
}

}