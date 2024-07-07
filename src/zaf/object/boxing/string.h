#pragma once

#include <zaf/object/boxing/custom_boxing_traits.h>
#include <zaf/object/boxing/internal/boxed_represent.h>
#include <zaf/object/boxing/string_parser.h>
#include <zaf/object/object.h>

namespace zaf {

#define __ZAF_INTERNAL_DEFINE_STRING_BOXED_TYPE(NonBoxedType, BoxedTypeName)                      \
class BoxedTypeName : public Object, public internal::BoxedRepresent<NonBoxedType> {              \
public:                                                                                           \
    ZAF_OBJECT;                                                                                   \
    using BoxedRepresent<NonBoxedType>::BoxedRepresent;                                           \
    bool IsEqual(const Object& other) const override;                                             \
    std::size_t Hash() const override;                                                            \
    std::wstring ToString() const override;                                                       \
    void CloneFrom(const Object& other) override;                                                 \
};                                                                                                \
ZAF_OBJECT_BEGIN(BoxedTypeName);                                                                  \
ZAF_OBJECT_PARSER(StringParser<BoxedTypeName>);                                                   \
ZAF_OBJECT_END;                                                                                   \
/* CustomBoxingTraits for string */                                                               \
template<>                                                                                        \
struct zaf__CustomBoxingTraits<NonBoxedType> {                                                    \
    using BoxedType = BoxedTypeName;                                                              \
    static std::shared_ptr<BoxedType> Box(NonBoxedType value) {                                   \
        return std::make_shared<BoxedType>(std::move(value));                                     \
    }                                                                                             \
    static const NonBoxedType* Unbox(const BoxedType& object) {                                   \
        return &object.Value();                                                                   \
    }                                                                                             \
};                                                                                                \
/* CustomBoxingTraits for string view */                                                          \
template<>                                                                                        \
struct zaf__CustomBoxingTraits<std::basic_string_view<NonBoxedType::value_type>> {                \
    using BoxedType = BoxedTypeName;                                                              \
    static std::shared_ptr<BoxedType> Box(                                                        \
        std::basic_string_view<NonBoxedType::value_type> value) {                                 \
        return std::make_shared<BoxedType>(NonBoxedType{ value });                                \
    }                                                                                             \
};                                                                                                \
/* CustomBoxingTraits for char pointer */                                                         \
template<>                                                                                        \
struct zaf__CustomBoxingTraits<NonBoxedType::value_type*> {                                       \
    using BoxedType = BoxedTypeName;                                                              \
    static std::shared_ptr<BoxedType> Box(NonBoxedType::value_type* value) {                      \
        return std::make_shared<BoxedType>(NonBoxedType{ value });                                \
    }                                                                                             \
};                                                                                                \
/* CustomBoxingTraits for const char pointer */                                                   \
template<>                                                                                        \
struct zaf__CustomBoxingTraits<const NonBoxedType::value_type*> {                                 \
    using BoxedType = BoxedTypeName;                                                              \
    static std::shared_ptr<BoxedType> Box(const NonBoxedType::value_type* value) {                \
        return std::make_shared<BoxedType>(NonBoxedType{ value });                                \
    }                                                                                             \
};


__ZAF_INTERNAL_DEFINE_STRING_BOXED_TYPE(std::string, String)
__ZAF_INTERNAL_DEFINE_STRING_BOXED_TYPE(std::wstring, WideString)

}