#pragma once

#include <zaf/base/flags_enum.h>
#include <zaf/creation.h>
#include <zaf/internal/object/enum_constant_registrar.h>
#include <zaf/internal/object/enum_parser.h>
#include <zaf/internal/object/enum_value_combiner.h>
#include <zaf/dynamic/boxing/custom_boxing_traits.h>
#include <zaf/dynamic/boxing/internal/boxed_represent.h>
#include <zaf/dynamic/boxing/internal/boxed_represent_equal.h>
#include <zaf/dynamic/enum_constant.h>
#include <zaf/dynamic/enum_type.h>
#include <zaf/dynamic/internal/reflection_manager.h>
#include <zaf/dynamic/object.h>

#define ZAF_ENUM_BEGIN(EnumName)                                                                  \
class EnumName##Enum : public zaf::Object, public zaf::internal::BoxedRepresent<EnumName> {       \
public:                                                                                           \
    ZAF_OBJECT;                                                                                   \
    static zaf::EnumType* EnumType();                                                             \
    using BoxedRepresent<EnumName>::BoxedRepresent;                                               \
    bool IsEqual(const zaf::Object& other) const override;                                        \
    std::size_t Hash() const override;                                                            \
    std::wstring ToString() const override;                                                       \
};                                                                                                \
template<>                                                                                        \
struct zaf__CustomBoxingTraits<EnumName> {                                                        \
    using BoxedType = EnumName##Enum;                                                             \
    static std::shared_ptr<BoxedType> Box(EnumName value) {                                       \
        return std::make_shared<BoxedType>(value);                                                \
    }                                                                                             \
    static const EnumName* Unbox(const BoxedType& object) {                                       \
        return &object.Value();                                                                   \
    }                                                                                             \
};                                                                                                \
class EnumName##Enum::Type : public zaf::EnumType {                                               \
private:                                                                                          \
    using DeclaredType = EnumName;                                                                \
    using Class = EnumName##Enum;                                                                 \
    static Type instance;                                                                         \
    Type();                                                                                       \
public:                                                                                           \
    static Type* Instance() { return &instance; }                                                 \
    zaf::dynamic::ObjectType* BaseType() const noexcept override {                                         \
        return Class::StaticBaseType();                                                           \
    }                                                                                             \
    std::wstring_view Name() const noexcept override {                                            \
        return std::wstring_view{ L#EnumName };                                                   \
    }                                                                                             \
    std::shared_ptr<zaf::Object> CreateInstance() const override {                                \
        return zaf::Create<EnumName##Enum>();                                                     \
    }                                                                                             \
    zaf::ObjectParser* Parser() const override {                                                  \
        static zaf::internal::EnumParser parser(this);                                            \
        return &parser;                                                                           \
    }                                                                                             \
    void SetValue(Object& result, const Object& value) const override {                           \
        As<Class>(result).Value() = As<Class>(value).Value();                                     \
    }                                                                                             \
    bool IsFlagsEnum() const noexcept override {                                                  \
        return zaf::IsFlagsEnumV<EnumName>;                                                       \
    }                                                                                             \
    void CombineFlagValue(Object& result, const Object& value) const override {                   \
        zaf::internal::EnumValueCombiner<DeclaredType, Class>::Combine(result, value);            \
    }


#define ZAF_ENUM_CONSTANT(ConstantName)                                                           \
private:                                                                                          \
    class ConstantName##ConstantType : public zaf::EnumConstant {                                 \
    public:                                                                                       \
        const std::wstring& Name() const override {                                               \
            static const std::wstring name{ L#ConstantName };                                     \
            return name;                                                                          \
        }                                                                                         \
        zaf::dynamic::ObjectType* ValueType() const override {                                             \
            return Class::StaticType();                                                           \
        }                                                                                         \
        std::shared_ptr<Object> Value() const override {                                          \
            return std::make_shared<Class>(DeclaredType::ConstantName);                           \
        }                                                                                         \
    };                                                                                            \
public:                                                                                           \
    zaf::EnumConstant* const ConstantName##Constant = []() {                                      \
        static ConstantName##ConstantType constant;                                               \
        zaf::internal::EnumConstantRegistrar::Register(Class::EnumType(), &constant);             \
        return &constant;                                                                         \
    }();


#define ZAF_ENUM_END                                                                              \
}; /* Type */


#define ZAF_ENUM_IMPL(EnumName)                                                                   \
EnumName##Enum::Type EnumName##Enum::Type::instance;                                              \
EnumName##Enum::Type::Type() {                                                                    \
    zaf::internal::ReflectionManager::Instance().RegisterType(this);                              \
}                                                                                                 \
zaf::dynamic::ObjectType* EnumName##Enum::StaticType() { return Type::Instance(); }                        \
zaf::dynamic::ObjectType* EnumName##Enum::DynamicType() const { return Type::Instance(); }                 \
zaf::dynamic::ObjectType* EnumName##Enum::StaticBaseType() { return __super::StaticType(); }               \
zaf::EnumType* EnumName##Enum::EnumType() {                                                       \
    return Type::Instance();                                                                      \
}                                                                                                 \
bool EnumName##Enum::IsEqual(const zaf::Object& other) const {                                    \
    return zaf::internal::BoxedRepresentEqual<EnumName##Enum>(*this, other);                      \
}                                                                                                 \
std::size_t EnumName##Enum::Hash() const {                                                        \
    auto value = static_cast<std::underlying_type_t<EnumName>>(Value());                          \
    return std::hash<std::underlying_type_t<EnumName>>()(value);                                  \
}                                                                                                 \
std::wstring EnumName##Enum::ToString() const {                                                   \
    auto name = Type::Instance()->GetValueName(*this);                                            \
    if (name.empty()) {                                                                           \
        auto value = static_cast<std::underlying_type_t<EnumName>>(this->Value());                \
        name = std::to_wstring(value);                                                            \
    }                                                                                             \
    return name;                                                                                  \
}