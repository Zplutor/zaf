#pragma once

#include <zaf/application.h>
#include <zaf/creation.h>
#include <zaf/reflection/reflection_manager.h>

#define ZAF_DEFINE_REFLECTION_TYPE(ClassName, BaseClassName)                                    \
class ClassName##Type : public ReflectionType {                                                 \
public:                                                                                         \
    std::wstring GetName() override {                                                           \
        return L#ClassName;                                                                     \
    }                                                                                           \
    std::shared_ptr<ReflectionObject> CreateInstance() override {                               \
        return Create<ClassName>();                                                             \
    }                                                                                           \
    std::shared_ptr<Parser> GetParser() override {                                              \
        return std::make_shared<ClassName##Parser>();                                           \
    }                                                                                           \
    std::shared_ptr<ReflectionType> GetBase() override {                                        \
        return BaseClassName::Type;                                                             \
    }                                                                                           \
};                                                                                              \
std::shared_ptr<ReflectionType> Create##ClassName##Type() {                                     \
    auto type = std::make_shared<ClassName##Type>();                                            \
    GetReflectionManager().RegisterType(type);                                                  \
    return type;                                                                                \
}                                                                                               \
const std::shared_ptr<ReflectionType> ClassName::Type = Create##ClassName##Type(); 
    