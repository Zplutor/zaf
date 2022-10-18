#include <zaf/graphic/size.h>
#include <zaf/base/as.h>
#include <zaf/object/type_definition.h>
#include <zaf/object/parsing/internal/utility.h>

namespace zaf {
namespace {

class SizeParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, Object& object) override {

        auto& size = As<Size>(object);
        internal::ParseAttributeToDoubleFloats(attribute_value, size.width, size.height);
    }

    void ParseFromNode(const XamlNode& node, Object& object) override {

    }
};

}

ZAF_DEFINE_TYPE(Size)
ZAF_DEFINE_TYPE_PARSER(SizeParser)
ZAF_DEFINE_TYPE_PROPERTY(Width)
ZAF_DEFINE_TYPE_PROPERTY(Height)
ZAF_DEFINE_TYPE_END

ZAF_DEFINE_EQUALITY(Size)

}