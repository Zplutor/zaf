#pragma once

#include <zaf/object/internal/property_deduction.h>
#include <zaf/object/internal/property_registrar.h>
#include <zaf/object/object_property.h>

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
class PropertyName##Property : public zaf::ObjectProperty {                                     \
public:                                                                                         \
	const std::wstring& GetName() const override {                                              \
        static const std::wstring name{ L#PropertyName };                                       \
		return name;                                                                            \
	}                                                                                           \
    ObjectType* GetValueType() const override {                                                 \
		return nullptr;                                                                         \
	}                                                                                           \
    std::any GetValue(const Object& object) const override {                                    \
        return PropertyName##Accessor::Get(dynamic_cast<const Class&>(object), nullptr);        \
    }                                                                                           \
    void SetValue(Object& object, const std::any& value) const override {                       \
        return PropertyName##Accessor::Set(dynamic_cast<Class&>(object), value, nullptr);       \
    }                                                                                           \
};                                                                                              \
ObjectProperty* const PropertyName##_property =                                                 \
    [this]() {                                                                                  \
        static PropertyName##Property property;                                                 \
        internal::PropertyRegistrar::Register(this, &property);                                 \
        return &property;                                                                       \
    }();