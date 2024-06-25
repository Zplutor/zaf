#pragma once

#include <zaf/object/boxing/custom_boxed_type.h>
#include <zaf/object/boxing/internal/boxed_represent.h>
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


template<>
struct GetCustomBoxedType<std::string> {
    using type = String;
};

template<>
struct GetCustomBoxedType<char*> {
    using type = String;
};

template<>
struct GetCustomBoxedType<const char*> {
    using type = String;
};


template<>
struct GetCustomBoxedType<std::wstring> {
    using type = WideString;
};

template<>
struct GetCustomBoxedType<wchar_t*> {
    using type = WideString;
};

template<>
struct GetCustomBoxedType<const wchar_t*> {
    using type = WideString;
};

}