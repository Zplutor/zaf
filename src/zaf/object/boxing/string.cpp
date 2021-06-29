#include <zaf/object/boxing/string.h>
#include <zaf/object/type_definition.h>
#include <zaf/object/boxing/internal/boxed_represent_equal.h>
#include <zaf/object/boxing/internal/string_conversion_shim.h>
#include <zaf/parsing/object_parser.h>
#include <zaf/parsing/xaml_node.h>

namespace zaf {
namespace {

template<typename T>
class StringParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& value, Object& object) override {

        auto& string = dynamic_cast<T&>(object);
        string.SetValue(internal::StringConversionShim<typename T::ValueType>::From(value));
    }

    void ParseFromNode(const XamlNode& node, Object& object) override {

        auto& string = dynamic_cast<T&>(object);

        const auto& content_nodes = node.GetContentNodes();
        if (content_nodes.size() != 1) {
            //TODO: Raise a runtime error.
        }

        const auto& first_node = content_nodes.front();
        if (first_node->GetType() != XamlNode::Type::Text) {
            //TODO: Raise a runtime error.
        }

        string.SetValue(
            internal::StringConversionShim<typename T::ValueType>::From(first_node->GetValue()));
    }
};

}


#define ZAF_INTERNAL_IMPLEMENT_STRING_BOXED_TYPE(BoxedTypeName)                                  \
ZAF_DEFINE_TYPE(BoxedTypeName)                                                                   \
ZAF_DEFINE_PARSER(StringParser<BoxedTypeName>)                                                   \
ZAF_DEFINE_END                                                                                   \
bool BoxedTypeName::IsEqual(const Object& other) const {                                         \
    return internal::BoxedRepresentEqual<BoxedTypeName>(*this, other);                           \
}                                                                                                \
std::size_t BoxedTypeName::Hash() const  {                                                       \
    return std::hash<typename BoxedTypeName::ValueType>()(Value());                              \
}                                                                                                \
std::wstring BoxedTypeName::ToString() const {                                                   \
    return internal::StringConversionShim<std::wstring>::From(Value());                          \
}


ZAF_INTERNAL_IMPLEMENT_STRING_BOXED_TYPE(String)
ZAF_INTERNAL_IMPLEMENT_STRING_BOXED_TYPE(WideString)

}