#include <zaf/object/boxing/boolean.h>
#include <zaf/base/as.h>
#include <zaf/object/parsing/parse_error.h>
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

        __super::ParseFromNode(node, object);

        auto content_string = GetContentStringFromXamlNode(node);
        if (!content_string) {
            throw ParseError{ ZAF_SOURCE_SITE() };
        }

        Parse(*content_string, object);
    }

private:
    void Parse(const std::wstring& string, Object& object) {

        auto& boolean = As<Boolean>(object);

        if (string == L"true") {
            boolean.SetValue(true);
        }
        else if (string == L"false") {
            boolean.SetValue(false);
        }
        else {
            throw ParseError{ ZAF_SOURCE_SITE() };
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