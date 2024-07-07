#include <zaf/object/boxing/numeric.h>
#include <functional>
#include <zaf/base/as.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/base/string/to_string.h>
#include <zaf/object/boxing/internal/boxed_represent_equal.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_utility.h>

namespace zaf {

#define ZAF_INTERNAL_IMPLEMENT_NUMERIC_BOXED_TYPE(BoxedTypeName)                                  \
ZAF_OBJECT_IMPL(BoxedTypeName)                                                                    \
 bool BoxedTypeName::IsEqual(const Object& other) const {                                         \
    return internal::BoxedRepresentEqual(*this, other);                                           \
}                                                                                                 \
std::size_t BoxedTypeName::Hash() const {                                                         \
    return std::hash<typename BoxedTypeName::ValueType>()(Value());                               \
}                                                                                                 \
std::wstring BoxedTypeName::ToString() const {                                                    \
    return zaf::ToWideString(Value());                                                            \
}                                                                                                 \
void BoxedTypeName::CloneFrom(const Object& other) {                                              \
    *this = As<BoxedTypeName>(other);                                                             \
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