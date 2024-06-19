#include <zaf/graphic/size.h>
#include <zaf/base/as.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/base/string/to_string.h>
#include <zaf/object/type_definition.h>
#include <zaf/object/parsing/internal/utility.h>

namespace zaf {
namespace {

class SizeParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, Object& object) override {

        auto& size = As<Size>(object);

        bool is_succeeded = internal::ParseAttributeToDoubleFloats(
            attribute_value, 
            size.width, 
            size.height);

        if (!is_succeeded) {
            throw ParseError{ ZAF_SOURCE_LOCATION() };
        }
    }
};

}

ZAF_DEFINE_TYPE(Size)
ZAF_DEFINE_TYPE_PARSER(SizeParser)
ZAF_DEFINE_TYPE_END

//ZAF_DEFINE_PROPERTY(Size);

ZAF_DEFINE_EQUALITY(Size)


std::wstring Size::ToString() const {
    return ToWideString(width) + L',' + ToWideString(height);
}

}