#include <zaf/object/boxing/numeric.h>
#include <functional>
#include <zaf/base/as.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/base/string/to_string.h>
#include <zaf/object/boxing/internal/boxed_represent_equal.h>
#include <zaf/object/type_definition.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_utility.h>

namespace zaf {
namespace {

template<typename T>
class NumericParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& value, Object& object) override {
        Parse(value, object);
    }

    void ParseFromNode(const XamlNode& node, Object& object) override {

        __super::ParseFromNode(node, object);

        auto content_string = GetContentStringFromXamlNode(node);
        if (!content_string) {
            throw ParseError{ ZAF_SOURCE_LOCATION() };
        }

        Parse(*content_string, object);
    }

private:
    void Parse(const std::wstring& string, Object& object) {

        typename T::ValueType value{};
        if (TryToNumeric<typename T::ValueType>(string, value)) {
            As<T>(object).SetValue(value);
        }
        else {
            throw ParseError{ ZAF_SOURCE_LOCATION() };
        }
    }
};

}


#define ZAF_INTERNAL_IMPLEMENT_NUMERIC_BOXED_TYPE(BoxedTypeName)                                   \
ZAF_DEFINE_TYPE(BoxedTypeName)                                                                     \
ZAF_DEFINE_TYPE_PARSER(NumericParser<BoxedTypeName>)                                               \
ZAF_DEFINE_TYPE_END                                                                                \
 bool BoxedTypeName::IsEqual(const Object& other) const {                                          \
    return internal::BoxedRepresentEqual(*this, other);                                            \
}                                                                                                  \
std::size_t BoxedTypeName::Hash() const {                                                          \
    return std::hash<typename BoxedTypeName::ValueType>()(Value());                                \
}                                                                                                  \
std::wstring BoxedTypeName::ToString() const {                                                     \
    return zaf::ToWideString(Value());                                                             \
}                                                                                                


ZAF_INTERNAL_IMPLEMENT_NUMERIC_BOXED_TYPE(Char)
ZAF_INTERNAL_IMPLEMENT_NUMERIC_BOXED_TYPE(WideChar)
ZAF_INTERNAL_IMPLEMENT_NUMERIC_BOXED_TYPE(Int8)
ZAF_INTERNAL_IMPLEMENT_NUMERIC_BOXED_TYPE(UInt8)
ZAF_INTERNAL_IMPLEMENT_NUMERIC_BOXED_TYPE(Int16)
ZAF_INTERNAL_IMPLEMENT_NUMERIC_BOXED_TYPE(UInt16)
ZAF_INTERNAL_IMPLEMENT_NUMERIC_BOXED_TYPE(Int32)
ZAF_INTERNAL_IMPLEMENT_NUMERIC_BOXED_TYPE(UInt32)
ZAF_INTERNAL_IMPLEMENT_NUMERIC_BOXED_TYPE(Int64)
ZAF_INTERNAL_IMPLEMENT_NUMERIC_BOXED_TYPE(UInt64)
ZAF_INTERNAL_IMPLEMENT_NUMERIC_BOXED_TYPE(Float)
ZAF_INTERNAL_IMPLEMENT_NUMERIC_BOXED_TYPE(Double)

}