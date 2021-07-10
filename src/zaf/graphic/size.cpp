#include <zaf/graphic/size.h>
#include <zaf/object/type_definition.h>
#include <zaf/parsing/parsers/internal/utility.h>

namespace zaf {
namespace {

class SizeParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, Object& object) override {

        auto& size = dynamic_cast<Size&>(object);
        internal::ParseAttributeToDoubleFloats(attribute_value, size.width, size.height);
    }
};

}

ZAF_DEFINE_TYPE(Size)
ZAF_DEFINE_TYPE_PARSER(SizeParser)
ZAF_DEFINE_TYPE_PROPERTY_WITH_FIELD(Width, width)
ZAF_DEFINE_TYPE_PROPERTY_WITH_FIELD(Height, height)
ZAF_DEFINE_TYPE_END

ZAF_DEFINE_EQUALITY(Size)

}