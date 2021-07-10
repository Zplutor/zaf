#include <zaf/object/boxing/numeric.h>
#include <functional>
#include <zaf/base/string/to_numeric.h>
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
        Parse(GetContentStringFromXamlNode(node), object);
    }

private:
    void Parse(const std::wstring& string, Object& object) {

        auto& numeric = dynamic_cast<T&>(object);

        typename T::ValueType value{};
        if (TryToNumeric<typename T::ValueType>(string, value)) {
            numeric.SetValue(value);
        }
        else {
            //TODO: Raise a runtime error.
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
    return std::to_wstring(Value());                                                               \
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