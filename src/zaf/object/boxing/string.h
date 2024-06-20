#pragma once

#include <zaf/object/boxing/internal/boxed_represent.h>
#include <zaf/object/boxing/internal/built_in_box_types.h>
#include <zaf/object/boxing/string_parser.h>
#include <zaf/object/object.h>

namespace zaf {

#define __ZAF_INTERNAL_DEFINE_STRING_BOXED_TYPE(StringType, BoxedTypeName)                        \
class BoxedTypeName : public Object, public internal::BoxedRepresent<StringType> {                \
public:                                                                                           \
    ZAF_OBJECT;                                                                                   \
    using BoxedRepresent<StringType>::BoxedRepresent;                                             \
    bool IsEqual(const Object& other) const override;                                             \
    std::size_t Hash() const override;                                                            \
    std::wstring ToString() const override;                                                       \
};                                                                                                \
ZAF_OBJECT_BEGIN(BoxedTypeName);                                                                  \
ZAF_OBJECT_PARSER(StringParser<BoxedTypeName>);                                                   \
ZAF_OBJECT_END;


__ZAF_INTERNAL_DEFINE_STRING_BOXED_TYPE(std::string, String)
__ZAF_INTERNAL_DEFINE_STRING_BOXED_TYPE(std::wstring, WideString)


__ZAF_INTERNAL_DEFINE_BUILT_IN_BOXED_TYPE(std::string, String)
__ZAF_INTERNAL_DEFINE_BUILT_IN_BOXED_TYPE(std::wstring, WideString)
__ZAF_INTERNAL_DEFINE_BUILT_IN_BOXED_TYPE(char*, String)
__ZAF_INTERNAL_DEFINE_BUILT_IN_BOXED_TYPE(const char*, String)
__ZAF_INTERNAL_DEFINE_BUILT_IN_BOXED_TYPE(wchar_t*, WideString)
__ZAF_INTERNAL_DEFINE_BUILT_IN_BOXED_TYPE(const wchar_t*, WideString)

}