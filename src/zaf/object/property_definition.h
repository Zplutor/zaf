#pragma once

#include <zaf/object/boxing/internal/get_box_type.h>
#include <zaf/object/internal/property_deduction.h>
#include <zaf/object/internal/property_registrar.h>
#include <zaf/object/object_property.h>

#define ZAF_DEFINE_PROPERTY(PropertyName)                                                          \
struct PropertyName##Accessor {                                                                    \
    template<typename T>                                                                           \
    using GetterValueType =                                                                        \
        typename zaf::internal::DeduceGetterType<decltype(&T::PropertyName)>::Type;                \
    template<typename T>                                                                           \
    using SetterValueType =                                                                        \
        typename zaf::internal::DeduceSetterType<decltype(&T::##PropertyName)>::Type;              \
    template<typename T>                                                                           \
    static std::any Get(                                                                           \
        const T& object,                                                                           \
        GetterValueType<T>*) {                                                                     \
        return object.PropertyName();                                                              \
    }                                                                                              \
    template<typename T>                                                                           \
    static std::any Get(const T& object, ...) {                                                    \
        throw std::exception{};                                                                    \
    }                                                                                              \
    template<typename T>                                                                           \
    static void Set(                                                                               \
        T& object,                                                                                 \
        const std::any& value,                                                                     \
        SetterValueType<T>* value_type) {                                                          \
        object.Set##PropertyName(std::any_cast<decltype(*value_type)>(value));                     \
    }                                                                                              \
    template<typename T>                                                                           \
    static void Set(T& object, const std::any& value, ...) {                                       \
        throw std::exception{};                                                                    \
    }                                                                                              \
    template<typename T>                                                                           \
    static constexpr GetterValueType<T>* DeduceValueType(std::nullptr_t) {                         \
        return nullptr;                                                                            \
    }                                                                                              \
    template<typename T>                                                                           \
    static constexpr SetterValueType<T>* DeduceValueType(void*) {                                  \
        return nullptr;                                                                            \
    }                                                                                              \
    using ValueType = decltype(*DeduceValueType<Class>(nullptr));                                  \
};                                                                                                 \
class PropertyName##Property : public zaf::ObjectProperty {                                        \
public:                                                                                            \
	const std::wstring& GetName() const override {                                                 \
        static const std::wstring name{ L#PropertyName };                                          \
		return name;                                                                               \
	}                                                                                              \
    ObjectType* GetValueType() const override {                                                    \
		return zaf::internal::GetBoxType<typename PropertyName##Accessor::ValueType>::Type::Type;  \
	}                                                                                              \
    std::any GetValue(const zaf::Object& object) const override {                                  \
        return PropertyName##Accessor::Get(dynamic_cast<const Class&>(object), nullptr);           \
    }                                                                                              \
    void SetValue(zaf::Object& object, const std::any& value) const override {                     \
        return PropertyName##Accessor::Set(dynamic_cast<Class&>(object), value, nullptr);          \
    }                                                                                              \
};                                                                                                 \
zaf::ObjectProperty* const PropertyName##_property =                                               \
    [this]() {                                                                                     \
        static PropertyName##Property property;                                                    \
        zaf::internal::PropertyRegistrar::Register(this, &property);                               \
        return &property;                                                                          \
    }();