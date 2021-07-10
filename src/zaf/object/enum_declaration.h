#pragma once

#include <zaf/object/boxing/internal/boxed_represent.h>
#include <zaf/object/boxing/internal/built_in_box_types.h>
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
__ZAF_INTERNAL_DEFINE_BUILT_IN_BOXED_TYPE(EnumName, EnumName##Enum)


#define ZAF_ENABLE_ENUM_BOXING_WITH_NAMESPACE(Namespace, EnumName) \
__ZAF_INTERNAL_DEFINE_BUILT_IN_BOXED_TYPE(Namespace::EnumName, Namespace::EnumName##Enum)