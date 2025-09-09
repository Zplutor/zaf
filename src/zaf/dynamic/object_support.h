#pragma once

#include <zaf/dynamic/internal/instance_creator.h>
#include <zaf/dynamic/internal/reflection_manager.h>
#include <zaf/dynamic/object_type.h>

#define ZAF_OBJECT                                                                                \
class Type;                                                                                       \
static zaf::dynamic::ObjectType* StaticType();                                                    \
zaf::dynamic::ObjectType* DynamicType() const override;                                           \
static zaf::dynamic::ObjectType* StaticBaseType();


#define ZAF_OBJECT_BEGIN(ClassName)                                                               \
class ClassName::Type : public zaf::dynamic::ObjectType {                                         \
private:                                                                                          \
    using Class = ClassName;                                                                      \
    static Type instance;                                                                         \
    Type();                                                                                       \
public:                                                                                           \
    static Type* Instance() { return &instance; }                                                 \
    zaf::dynamic::ObjectType* BaseType() const noexcept override {                                \
        return ClassName::StaticBaseType();                                                       \
    }                                                                                             \
    std::wstring_view Name() const noexcept override {                                            \
        return std::wstring_view{ L#ClassName };                                                  \
    }                                                                                             \
    std::shared_ptr<zaf::dynamic::Object> CreateInstance() const override {                                \
        return zaf::internal::InstanceCreatorSelector<ClassName>::Type::Create();                 \
    }


#define ZAF_OBJECT_PARSER(ParserType)                                                             \
    zaf::ObjectParser* Parser() const override {                                                  \
        static ParserType parser;                                                                 \
        return &parser;                                                                           \
    }


#define ZAF_OBJECT_RESOURCE_URI(URI)                                                              \
    std::wstring_view ResourceURI() const noexcept override {                                     \
        return std::wstring_view{ URI };                                                          \
    }                                                                                           


#define ZAF_OBJECT_END };


#define ZAF_OBJECT_IMPL(ClassName)                                                                \
ClassName::Type ClassName::Type::instance;                                                        \
ClassName::Type::Type() {                                                                         \
    zaf::internal::ReflectionManager::Instance().RegisterType(this);                              \
}                                                                                                 \
zaf::dynamic::ObjectType* ClassName::StaticType() { return Type::Instance(); }                    \
zaf::dynamic::ObjectType* ClassName::DynamicType() const { return Type::Instance(); }             \
zaf::dynamic::ObjectType* ClassName::StaticBaseType() { return __super::StaticType(); }
