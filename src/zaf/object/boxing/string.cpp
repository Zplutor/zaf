#include <zaf/object/boxing/string.h>
#include <zaf/base/as.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/object/type_definition.h>
#include <zaf/object/boxing/internal/boxed_represent_equal.h>
#include <zaf/object/boxing/internal/string_conversion_shim.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/parsing/xaml_utility.h>

namespace zaf {
namespace {

template<typename T>
class StringParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& value, Object& object) override {
        Parse(value, object);
    }

    void ParseFromNode(const XamlNode& node, Object& object) override {

        __super::ParseFromNode(node, object);

        if (node.GetContentNodes().empty()) {
            As<T>(object).SetValue(typename T::ValueType{});
            return;
        }

        auto content_string = GetContentStringFromXamlNode(node);
        if (!content_string) {
            throw ParseError{ ZAF_SOURCE_SITE() };
        }

        Parse(*content_string, object);
    }

private:
    void Parse(const std::wstring& value, Object& object) {

        As<T>(object).SetValue(internal::StringConversionShim<typename T::ValueType>::From(value));
    }
};

}


#define ZAF_INTERNAL_IMPLEMENT_STRING_BOXED_TYPE(BoxedTypeName)                                    \
ZAF_DEFINE_TYPE(BoxedTypeName)                                                                     \
ZAF_DEFINE_TYPE_PARSER(StringParser<BoxedTypeName>)                                                \
ZAF_DEFINE_TYPE_END                                                                                \
bool BoxedTypeName::IsEqual(const Object& other) const {                                           \
    return internal::BoxedRepresentEqual<BoxedTypeName>(*this, other);                             \
}                                                                                                  \
std::size_t BoxedTypeName::Hash() const  {                                                         \
    return std::hash<typename BoxedTypeName::ValueType>()(Value());                                \
}                                                                                                  \
std::wstring BoxedTypeName::ToString() const {                                                     \
    return internal::StringConversionShim<std::wstring>::From(Value());                            \
}


ZAF_INTERNAL_IMPLEMENT_STRING_BOXED_TYPE(String)
ZAF_INTERNAL_IMPLEMENT_STRING_BOXED_TYPE(WideString)

}