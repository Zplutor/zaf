#include <zaf/object/boxing/boolean.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_utility.h>
#include <zaf/object/type_definition.h>

namespace zaf {
namespace {

class BooleanParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& value, Object& object) override {
        Parse(value, object);
    }

    void ParseFromNode(const XamlNode& node, Object& object) override {

        auto content_string = GetContentStringFromXamlNode(node);
        if (!content_string) {
            ZAF_THROW_ERRC(BasicErrc::InvalidValue);
        }

        Parse(*content_string, object);
    }

private:
    void Parse(const std::wstring& string, Object& object) {

        auto& boolean = dynamic_cast<Boolean&>(object);

        if (string == L"true") {
            boolean.SetValue(true);
        }
        else if (string == L"false") {
            boolean.SetValue(false);
        }
        else {
            ZAF_THROW_ERRC(BasicErrc::InvalidValue);
        }
    }
};

}

ZAF_DEFINE_TYPE(Boolean)
ZAF_DEFINE_TYPE_PARSER(BooleanParser)
ZAF_DEFINE_TYPE_END


bool Boolean::IsEqual(const Object& other) const {
    return internal::BoxedRepresentEqual(*this, other);
}    


std::size_t Boolean::Hash() const {
    return std::hash<typename Boolean::ValueType>()(Value());                              
}


std::wstring Boolean::ToString() const {
    if (Value()) {
        return L"true";
    }
    else {
        return L"false";
    }
}

}