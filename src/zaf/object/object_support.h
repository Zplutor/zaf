#pragma once

#include <zaf/object/internal/instance_creator.h>
#include <zaf/object/internal/reflection_manager.h>
#include <zaf/object/object_type.h>

#define ZAF_OBJECT                                                                                \
class Type;                                                                                       \
static zaf::ObjectType* StaticType();                                                             \
zaf::ObjectType* DynamicType() const override;                                                    \
static zaf::ObjectType* StaticBaseType();


#define ZAF_OBJECT_BEGIN(ClassName)                                                               \
class ClassName::Type : public zaf::ObjectType {                                                  \
private:                                                                                          \
    using Class = ClassName;                                                                      \
    static Type instance;                                                                         \
    Type();                                                                                       \
public:                                                                                           \
    static Type* Instance() { return &instance; }                                                 \
    zaf::ObjectType* BaseType() const override {                                                  \
        return ClassName::StaticBaseType();                                                       \
    }                                                                                             \
    const std::wstring& Name() const override {                                                   \
        static const std::wstring name{ L#ClassName };                                            \
        return name;                                                                              \
    }                                                                                             \
    std::shared_ptr<zaf::Object> CreateInstance() const override {                                \
        return zaf::internal::InstanceCreatorSelector<ClassName>::Type::Create();                 \
    }


#define ZAF_OBJECT_PARSER(ParserType)                                                             \
    zaf::ObjectParser* Parser() const override {                                                  \
        static ParserType parser;                                                                 \
        return &parser;                                                                           \
    }


#define ZAF_OBJECT_RESOURCE_URI(URI)                                                              \
    const std::wstring& ResourceURI() const override {                                            \
        static const std::wstring uri{ URI };                                                     \
        return uri;                                                                               \
    }                                                                                           


#define ZAF_OBJECT_END };


#define ZAF_OBJECT_IMPL(ClassName)                                                                \
ClassName::Type ClassName::Type::instance;                                                        \
ClassName::Type::Type() {                                                                         \
    zaf::internal::ReflectionManager::Instance().RegisterType(this);                              \
}                                                                                                 \
zaf::ObjectType* ClassName::StaticType() { return Type::Instance(); }                             \
zaf::ObjectType* ClassName::DynamicType() const { return Type::Instance(); }                      \
zaf::ObjectType* ClassName::StaticBaseType() { return __super::StaticType(); }
