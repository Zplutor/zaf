#pragma once

#include <zaf/object/boxing/custom_boxing_traits.h>
#include <zaf/object/boxing/internal/boxed_represent.h>
#include <zaf/object/enum_type.h>
#include <zaf/object/object.h>

#define ZAF_DECLARE_ENUM(EnumName)                                                                 \
class EnumName##Enum : public zaf::Object, public zaf::internal::BoxedRepresent<EnumName> {        \
public:                                                                                            \
    ZAF_DECLARE_TYPE                                                                               \
    static zaf::EnumType* EnumType();                                                              \
    using BoxedRepresent<EnumName>::BoxedRepresent;                                                \
    bool IsEqual(const zaf::Object& other) const override;                                         \
    std::size_t Hash() const override;                                                             \
    std::wstring ToString() const override;                                                        \
};                                                        


#define ZAF_ENABLE_ENUM_BOXING(EnumName) \
template<> \
struct zaf__CustomBoxingTraits<EnumName> { \
    using BoxedType = EnumName##Enum; \
    static std::shared_ptr<BoxedType> Box(EnumName value) { \
        return std::make_shared<BoxedType>(value); \
    } \
    static const EnumName* Unbox(const BoxedType& object) {\
        return &object.Value(); \
    }\
};


#define ZAF_ENABLE_ENUM_BOXING_WITH_NAMESPACE(Namespace, EnumName) \
template<> \
struct zaf__CustomBoxingTraits<Namespace::EnumName> { \
    using BoxedType = Namespace::EnumName##Enum; \
    static std::shared_ptr<BoxedType> Box(Namespace::EnumName value) { \
        return std::make_shared<BoxedType>(value); \
    } \
    static const Namespace::EnumName* Unbox(const BoxedType& object) {\
        return &object.Value(); \
    }\
};