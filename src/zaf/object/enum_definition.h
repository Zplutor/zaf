#pragma once

#include <zaf/creation.h>
#include <zaf/object/boxing/internal/boxed_represent_equal.h>
#include <zaf/object/internal/base_enum_type.h>
#include <zaf/object/internal/enum_parser.h>
#include <zaf/object/internal/reflection_manager.h>

#define ZAF_DEFINE_ENUM(EnumName)                                                                  \
/* Forward declarations */                                                                         \
namespace __zaf_internal_##EnumName##_type_definition {                                            \
zaf::ObjectType* CreateType();                                                                     \
zaf::EnumType* ToEnumType(zaf::ObjectType* type);                                                  \
std::wstring ToString(const EnumName##Enum& object);                                               \
}                                                                                                  \
zaf::ObjectType* const EnumName##Enum::Type =                                                      \
    __zaf_internal_##EnumName##_type_definition::CreateType();                                     \
zaf::EnumType* EnumName##Enum::EnumType() {                                                        \
    return __zaf_internal_##EnumName##_type_definition::ToEnumType(Type);                          \
}                                                                                                  \
bool EnumName##Enum::IsEqual(const zaf::Object& other) const {                                     \
    return zaf::internal::BoxedRepresentEqual<EnumName##Enum>(*this, other);                       \
}                                                                                                  \
std::size_t EnumName##Enum::Hash() const {                                                         \
    auto value = static_cast<std::underlying_type_t<EnumName>>(Value());                           \
    return std::hash<std::underlying_type_t<EnumName>>()(value);                                   \
}                                                                                                  \
std::wstring EnumName##Enum::ToString() const {                                                    \
    return __zaf_internal_##EnumName##_type_definition::ToString(*this);                           \
}                                                                                                  \
namespace __zaf_internal_##EnumName##_type_definition {                                            \
class TheType : public zaf::internal::BaseEnumType<EnumName> {                                     \
public:                                                                                            \
    using Class = EnumName;                                                                        \
    using ObjectClass = EnumName##Enum;                                                            \
    const std::wstring& GetName() const override {                                                 \
        static const std::wstring name{ L#EnumName };                                              \
        return name;                                                                               \
    }                                                                                              \
    std::shared_ptr<zaf::Object> CreateInstance() const override {                                 \
        return zaf::Create<EnumName##Enum>();                                                      \
    }                                                                                              \
protected:                                                                                         \
    zaf::ObjectParser* GetSelfParser() const override {                                            \
        static zaf::internal::EnumParser<EnumName> parser(this);                                   \
        return &parser;                                                                            \
    }                                                                                              \
    std::shared_ptr<zaf::Object> CreateObject(EnumName value) const override {                     \
        return zaf::Create<EnumName##Enum>(value);                                                 \
    }                                                                                              \
    const EnumName* UnboxValue(const zaf::Object& object) const override {                         \
        auto& enum_object = dynamic_cast<const EnumName##Enum&>(object);                           \
        return &enum_object.Value();                                                               \
    }                                                                                              \
    std::vector<std::pair<std::wstring_view, EnumName>> GenerateMap() const override {             \
        return std::vector<std::pair<std::wstring_view, EnumName>>{


#define ZAF_DEFINE_ENUM_VALUE(ValueName) { L#ValueName, Class::ValueName },


#define ZAF_DEFINE_ENUM_END                                                                        \
        };                                                                                         \
    }                                                                                              \
};                                                                                                 \
zaf::ObjectType* CreateType() {                                                                    \
    static TheType type;                                                                           \
    zaf::internal::ReflectionManager::Instance().RegisterType(&type);                              \
    return &type;                                                                                  \
}                                                                                                  \
zaf::EnumType* ToEnumType(zaf::ObjectType* type) {                                                 \
    return dynamic_cast<TheType*>(type);                                                           \
}                                                                                                  \
std::wstring ToString(const TheType::ObjectClass& object) {                                        \
    auto name = ToEnumType(object.Type)->GetValueName(object);                                     \
    if (name.empty()) {                                                                            \
        auto value = static_cast<std::underlying_type_t<TheType::Class>>(object.Value());          \
        name = std::to_wstring(value);                                                             \
    }                                                                                              \
    return name;                                                                                   \
}                                                                                                  \
}