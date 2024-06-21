#pragma once

#include <zaf/creation.h>
#include <zaf/object/boxing/internal/boxed_represent.h>
#include <zaf/object/boxing/internal/boxed_represent_equal.h>
#include <zaf/object/internal/base_enum_type.h>
#include <zaf/object/internal/enum_parser.h>
#include <zaf/object/internal/reflection_manager.h>
#include <zaf/object/object.h>

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
class EnumName##Enum::Type : public zaf::internal::BaseEnumType<EnumName> {                       \
private:                                                                                          \
    using DeclaredType = EnumName;                                                                \
    using Class = EnumName##Enum;                                                                 \
    static Type instance;                                                                         \
    Type();                                                                                       \
public:                                                                                           \
    static Type* Instance() { return &instance; }                                                 \
    const std::wstring& GetName() const override {                                                \
        static const std::wstring name{ L#EnumName };                                             \
        return name;                                                                              \
    }                                                                                             \
    std::shared_ptr<zaf::Object> CreateInstance() const override {                                \
        return zaf::Create<EnumName##Enum>();                                                     \
    }                                                                                             \
    zaf::ObjectParser* GetParser() const override {                                               \
        static zaf::internal::EnumParser<EnumName> parser(this);                                  \
        return &parser;                                                                           \
    }                                                                                             \
protected:                                                                                        \
    std::shared_ptr<zaf::Object> CreateObject(EnumName value) const override {                    \
        return zaf::Create<EnumName##Enum>(value);                                                \
    }                                                                                             \
    const EnumName* UnboxValue(const zaf::Object& object) const override {                        \
        auto& enum_object = dynamic_cast<const EnumName##Enum&>(object);                          \
        return &enum_object.Value();                                                              \
    }                                                                                             \
    std::vector<std::pair<std::wstring_view, EnumName>> GenerateMap() const override {            \
        return std::vector<std::pair<std::wstring_view, EnumName>>{


#define ZAF_ENUM_VALUE(ValueName) { L#ValueName, DeclaredType::ValueName },


#define ZAF_ENUM_END                                                                              \
        }; /* initialize list of std::vector*/                                                    \
    } /* GenerateMap() */                                                                         \
}; /* Type */


#define ZAF_ENUM_IMPL(EnumName)                                                                   \
EnumName##Enum::Type EnumName##Enum::Type::instance;                                              \
EnumName##Enum::Type::Type() {                                                                    \
    zaf::internal::ReflectionManager::Instance().RegisterType(this);                              \
}                                                                                                 \
zaf::ObjectType* EnumName##Enum::StaticType() { return Type::Instance(); }                        \
zaf::ObjectType* EnumName##Enum::DynamicType() const { return Type::Instance(); }                 \
zaf::ObjectType* EnumName##Enum::StaticBaseType() { return __super::StaticType(); }               \
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