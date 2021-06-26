#pragma once

#include <zaf/application.h>
#include <zaf/creation.h>
#include <zaf/reflection/reflection_manager.h>

#define ZAF_DEFINE_REFLECTION_TYPE(ClassName)                                                   \
namespace __zaf_internal_##ClassName##_type_definition {                                        \
    zaf::ObjectType* CreateType();  /* Forward declaration */                                   \
}                                                                                               \
                                                                                                \
zaf::ObjectType* const ClassName::Type =                                                        \
    __zaf_internal_##ClassName##_type_definition::CreateType();                                 \
                                                                                                \
namespace __zaf_internal_##ClassName##_type_definition {                                        \
class TheType : public zaf::ObjectType {                                                        \
public:                                                                                         \
    using Class = ClassName;                                                                    \
    zaf::ObjectType* GetBase() const override {                                                 \
        return reinterpret_cast<ClassName*>(0)->GetBaseType();                                  \
    }                                                                                           \
    const std::wstring& GetName() const override {                                              \
        static const std::wstring name{ L#ClassName };                                          \
        return name;                                                                            \
    }                                                                                           \
    std::shared_ptr<zaf::ReflectionObject> CreateInstance() const override {                    \
        return zaf::Create<ClassName>();                                                        \
    }                                                                                                                                                                                         

#define ZAF_DEFINE_PARSER(ParserType)                                                           \
    std::shared_ptr<zaf::Parser> GetParser() const override {                                   \
        static auto parser = std::make_shared<ParserType>();                                    \
        return parser;                                                                          \
    }                                                                                           

#define ZAF_DEFINE_RESOURCE_URI(Uri)                                                            \
    const std::wstring& GetResourceUri() const override {                                       \
        static const std::wstring uri{ Uri };                                                   \
        return uri;                                                                             \
    }                                                                                           

#define ZAF_DEFINE_END                                                                          \
};                                                                                              \
zaf::ObjectType* CreateType() {                                                                 \
    static TheType type;                                                                        \
    zaf::GetReflectionManager().RegisterType(&type);                                            \
    return &type;                                                                               \
}                                                                                               \
}