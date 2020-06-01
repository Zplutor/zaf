#pragma once

#include <zaf/application.h>
#include <zaf/creation.h>
#include <zaf/reflection/reflection_manager.h>

#define ZAF_DEFINE_REFLECTION_TYPE(ClassName)                                                   \
namespace __zaf_internal_##ClassName##TypeDefinition {                                          \
    std::shared_ptr<zaf::ReflectionType> CreateType();  /* Forward declaration */               \
}                                                                                               \
                                                                                                \
const std::shared_ptr<zaf::ReflectionType> ClassName::Type =                                    \
    __zaf_internal_##ClassName##TypeDefinition::CreateType();                                   \
                                                                                                \
namespace __zaf_internal_##ClassName##TypeDefinition {                                          \
class ObjectType : public zaf::ReflectionType {                                                 \
public:                                                                                         \
    std::shared_ptr<zaf::ReflectionType> GetBase() override {                                   \
        static auto prototype = std::make_shared<ClassName>();                                  \
        return prototype->GetBaseType();                                                        \
    }                                                                                           \
    std::wstring GetName() override {                                                           \
        return L#ClassName;                                                                     \
    }                                                                                           \
    std::shared_ptr<zaf::ReflectionObject> CreateInstance() override {                          \
        return zaf::Create<ClassName>();                                                        \
    }                                                                                                                                                                                         

#define ZAF_DEFINE_PARSER(ParserType)                                                           \
    std::shared_ptr<zaf::Parser> GetParser() override {                                         \
        static auto parser = std::make_shared<ParserType>();                                    \
        return parser;                                                                          \
    }                                                                                           

#define ZAF_DEFINE_RESOURCE_URI(Uri)                                                            \
    std::wstring GetResourceUri() override {                                                    \
        return Uri;                                                                             \
    }                                                                                           

#define ZAF_DEFINE_END                                                                          \
};                                                                                              \
std::shared_ptr<zaf::ReflectionType> CreateType() {                                             \
    auto type = std::make_shared<ObjectType>();                                                 \
    zaf::GetReflectionManager().RegisterType(type);                                             \
    return type;                                                                                \
}                                                                                               \
}