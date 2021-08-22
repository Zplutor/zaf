#include <zaf/graphic/frame.h>
#include <zaf/base/as.h>
#include <zaf/base/string/to_numeric.h>
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

        internal::ParseAttributeToQuaterFloats(
            value,
            frame.left,
            frame.top,
            frame.right,
            frame.bottom);
    }
};

}


ZAF_DEFINE_TYPE(Frame)
ZAF_DEFINE_TYPE_PARSER(FrameParser)
ZAF_DEFINE_TYPE_PROPERTY_WITH_FIELD(Left, left)
ZAF_DEFINE_TYPE_PROPERTY_WITH_FIELD(Top, top)
ZAF_DEFINE_TYPE_PROPERTY_WITH_FIELD(Right, right)
ZAF_DEFINE_TYPE_PROPERTY_WITH_FIELD(Bottom, bottom)
ZAF_DEFINE_TYPE_END

ZAF_DEFINE_EQUALITY(Frame)


}