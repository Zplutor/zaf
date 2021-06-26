#pragma once

#include <zaf/reflection/internal/property_deduction.h>
#include <zaf/reflection/internal/reflection_property_registrar.h>
#include <zaf/reflection/reflection_property.h>

#define ZAF_DEFINE_PROPERTY(PropertyName)                                                       \
struct PropertyName##Accessor {                                                                 \
    template<typename T>                                                                        \
    static std::any Get(                                                                        \
        const T& object,                                                                        \
        typename internal::DeduceGetterType<decltype(&T::PropertyName)>::Type*) {               \
        return object.PropertyName();                                                           \
    }                                                                                           \
    template<typename T>                                                                        \
    static std::any Get(const T& object, ...) {                                                 \
        throw std::exception{};                                                                 \
    }                                                                                           \
    template<typename T>                                                                        \
    static void Set(                                                                            \
        T& object,                                                                              \
        const std::any& value,                                                                  \
        typename internal::DeduceSetterType<decltype(&T::##PropertyName)>::Type* value_type) {  \
        object.Set##PropertyName(std::any_cast<decltype(*value_type)>(value));                  \
    }                                                                                           \
    template<typename T>                                                                        \
    static void Set(T& object, const std::any& value, ...) {                                    \
        throw std::exception{};                                                                 \
    }                                                                                           \
};                                                                                              \
class PropertyName##Property : public zaf::ReflectionProperty {                                 \
public:                                                                                         \
	const std::wstring& GetName() const override {                                              \
        static const std::wstring name{ L#PropertyName };                                       \
		return name;                                                                            \
	}                                                                                           \
    ObjectType* GetType() const override {                                                  \
		return nullptr;                                                                         \
	}                                                                                           \
    std::any GetValue(const ReflectionObject& object) const override {                          \
        return PropertyName##Accessor::Get(dynamic_cast<const Class&>(object), nullptr);        \
    }                                                                                           \
    void SetValue(ReflectionObject& object, const std::any& value) const override {             \
        return PropertyName##Accessor::Set(dynamic_cast<Class&>(object), value, nullptr);       \
    }                                                                                           \
};                                                                                              \
ReflectionProperty* const PropertyName##_property =                                             \
    [this]() {                                                                                  \
        static PropertyName##Property property;                                                 \
        internal::ReflectionPropertyRegistrar::Register(this, &property);                       \
        return &property;                                                                       \
    }();