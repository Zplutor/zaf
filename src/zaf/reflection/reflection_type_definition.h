#pragma once

#include <zaf/application.h>
#include <zaf/creation.h>
#include <zaf/reflection/reflection_manager.h>
#include <zaf/reflection/reflection_type.h>

#define ZAF_INTERNAL_DEFINE_REFLECTION_TYPE(TypeName, TypeNameString, ObjectClass)  \
class TypeName##Type : public ReflectionType {                                      \
public:                                                                             \
    std::wstring GetName() override {                                               \
        return TypeNameString;                                                      \
    }                                                                               \
    std::shared_ptr<ReflectionObject> CreateInstance() override {                   \
        return Create<ObjectClass>();                                               \
    }                                                                               \
    std::shared_ptr<Parser> GetParser() override {                                  \
        return std::make_shared<TypeName##Parser>();                                \
    }                                                                               \
};                                                                                  \
std::shared_ptr<ReflectionType> Create##TypeName##Type() {                          \
    auto type = std::make_shared<TypeName##Type>();                                 \
    GetReflectionManager().RegisterType(type);                                      \
    return type;                                                                    \
}                                                                                   \
const std::shared_ptr<ReflectionType> ObjectClass::Type = Create##TypeName##Type(); 


#define ZAF_DEFINE_REFLECTION_TYPE(ObjectClassName) \
    ZAF_INTERNAL_DEFINE_REFLECTION_TYPE(ObjectClassName, L#ObjectClassName, ObjectClassName)


#define ZAF_DEFINE_REFLECTION_TYPE_NESTED(OuterObjectClassName, ObjectClassName)  \
    ZAF_INTERNAL_DEFINE_REFLECTION_TYPE(                                          \
        OuterObjectClassName##ObjectClassName,                                    \
        L#OuterObjectClassName L"-" L#ObjectClassName,                            \
        OuterObjectClassName::ObjectClassName)