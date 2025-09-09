#include <zaf/dynamic/boxing/string.h>
#include <zaf/dynamic/boxing/internal/boxed_represent_equal.h>

namespace zaf {

#define ZAF_INTERNAL_IMPLEMENT_STRING_BOXED_TYPE(BoxedTypeName)                                   \
ZAF_OBJECT_IMPL(BoxedTypeName);                                                                   \
bool BoxedTypeName::IsEqual(const Object& other) const {                                          \
    return internal::BoxedRepresentEqual<BoxedTypeName>(*this, other);                            \
}                                                                                                 \
std::size_t BoxedTypeName::Hash() const  {                                                        \
    return std::hash<typename BoxedTypeName::ValueType>()(Value());                               \
}                                                                                                 \
std::wstring BoxedTypeName::ToString() const {                                                    \
    return internal::StringConversionShim<std::wstring>::From(Value());                           \
}


ZAF_INTERNAL_IMPLEMENT_STRING_BOXED_TYPE(String)
ZAF_INTERNAL_IMPLEMENT_STRING_BOXED_TYPE(WideString)

}