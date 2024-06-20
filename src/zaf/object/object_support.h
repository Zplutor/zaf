#pragma once

#include <zaf/object/internal/instance_creator.h>
#include <zaf/object/internal/reflection_manager.h>
#include <zaf/object/object_type.h>

#define ZAF_OBJECT \
class StaticType; \
zaf::ObjectType* GetType() const override;\
zaf::ObjectType* GetBaseType() const;


#define ZAF_OBJECT_BEGIN(ClassName) \
class ClassName::StaticType : public zaf::ObjectType { \
private:                                                                                          \
    using Class = ClassName;                                                                      \
    static StaticType instance;                                                                \
    StaticType();                                                                               \
public:                                                                                           \
    static StaticType* Instance() { return &instance; } \
    zaf::ObjectType* GetBase() const override {                                                 \
        return reinterpret_cast<ClassName*>(0)->GetBaseType();                                  \
    }                                                                                           \
    const std::wstring& GetName() const override {                                              \
        static const std::wstring name{ L#ClassName };                                          \
        return name;                                                                            \
    }                                                                                           \
    std::shared_ptr<zaf::Object> CreateInstance() const override {                              \
        return zaf::internal::InstanceCreatorSelector<ClassName>::Type::Create();               \
    }

#define ZAF_OBJECT_PARSER(ParserType)                                                           \
    zaf::ObjectParser* GetParser() const override {                                             \
        static ParserType parser;                                                               \
        return &parser;                                                                         \
    }

#define ZAF_OBJECT_RESOURCE_URI(URI)                                                            \
    const std::wstring& GetResourceURI() const override {                                       \
        static const std::wstring uri{ URI };                                                   \
        return uri;                                                                             \
    }                                                                                           

#define ZAF_OBJECT_END };

#define ZAF_OBJECT_IMPL(ClassName) \
ClassName::StaticType ClassName::StaticType::instance;\
ClassName::StaticType::StaticType() {\
    zaf::internal::ReflectionManager::Instance().RegisterType(this);\
}\
zaf::ObjectType* ClassName::GetType() const { return StaticType::Instance(); } \
zaf::ObjectType* ClassName::GetBaseType() const { \
    using BaseType = typename __super::StaticType;\
    return BaseType::Instance(); \
}