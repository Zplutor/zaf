#pragma once

#include <zaf/object/boxing/internal/get_box_type.h>
#include <zaf/object/internal/property_deduction.h>
#include <zaf/object/internal/property_registrar.h>
#include <zaf/object/object_property.h>

#define __ZAF_INTERNAL_DEFINE_PROPERTY_VARIABLE(PropertyName) \
zaf::ObjectProperty* const PropertyName##_property =                                               \
    [this]() {                                                                                     \
        static PropertyName##Property property;                                                    \
        /* "this" is the object type */                                                            \
        zaf::internal::PropertyRegistrar::Register(this, &property);                               \
        return &property;                                                                          \
    }();


#define ZAF_DEFINE_PROPERTY(PropertyName)                                                          \
struct PropertyName##Accessor {                                                                    \
    template<typename T>                                                                           \
    using GetterValueType =                                                                        \
        typename zaf::internal::DeduceGetterType<decltype(&T::PropertyName)>::Type;                \
    template<typename T>                                                                           \
    using SetterValueType =                                                                        \
        typename zaf::internal::DeduceSetterType<decltype(&T::Set##PropertyName)>::Type;           \
    template<typename T>                                                                           \
    static constexpr bool CanGet(GetterValueType<T>*) {                                            \
        return true;                                                                               \
    }                                                                                              \
    template<typename T>                                                                           \
    static constexpr bool CanGet(...) {                                                            \
        return false;                                                                              \
    }                                                                                              \
    template<typename T>                                                                           \
    static constexpr bool CanSet(SetterValueType<T>*) {                                            \
        return true;                                                                               \
    }                                                                                              \
    template<typename T>                                                                           \
    static constexpr bool CanSet(...) {                                                            \
        return false;                                                                              \
    }                                                                                              \
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
        object.Set##PropertyName(std::any_cast<std::decay_t<decltype(*value_type)>>(value));       \
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
    static constexpr SetterValueType<T>* DeduceValueType(...) {                                    \
        return nullptr;                                                                            \
    }                                                                                              \
    using ValueType = std::remove_pointer_t<decltype(DeduceValueType<Class>(nullptr))>;            \
};                                                                                                 \
class PropertyName##Property : public zaf::ObjectProperty {                                        \
public:                                                                                            \
	const std::wstring& GetName() const override {                                                 \
        static const std::wstring name{ L#PropertyName };                                          \
		return name;                                                                               \
	}                                                                                              \
    ObjectType* GetValueType() const override {                                                    \
        using ValueType =                                                                          \
            zaf::internal::GetBoxType<typename PropertyName##Accessor::ValueType>::Type;           \
        static_assert(zaf::internal::IsReflectionType<ValueType>::Value,                           \
            "This type of value is not supported by property.");                                   \
		return ValueType::Type;                                                                    \
	}                                                                                              \
    bool CanGet() const override {                                                                 \
        return PropertyName##Accessor::CanGet<Class>(nullptr);                                     \
    }                                                                                              \
    bool CanSet() const override {                                                                 \
        return PropertyName##Accessor::CanSet<Class>(nullptr);                                     \
    }                                                                                              \
    std::any GetValue(const zaf::Object& object) const override {                                  \
        return PropertyName##Accessor::Get(dynamic_cast<const Class&>(object), nullptr);           \
    }                                                                                              \
    void SetValue(zaf::Object& object, const std::any& value) const override {                     \
        return PropertyName##Accessor::Set(dynamic_cast<Class&>(object), value, nullptr);          \
    }                                                                                              \
};                                                                                                 \
__ZAF_INTERNAL_DEFINE_PROPERTY_VARIABLE(PropertyName)


#define ZAF_DEFINE_PROPERTY_WITH_FIELD(PropertyName, FieldName) \
class PropertyName##Property : public zaf::ObjectProperty {                                        \
public:                                                                                            \
	const std::wstring& GetName() const override {                                                 \
        static const std::wstring name{ L#PropertyName };                                          \
		return name;                                                                               \
	}                                                                                              \
    ObjectType* GetValueType() const override {                                                    \
        using ValueType =                                                                          \
            zaf::internal::GetBoxType<decltype(reinterpret_cast<Class*>(0)->FieldName)>::Type;     \
        static_assert(zaf::internal::IsReflectionType<ValueType>::Value,                           \
            "This type of value is not supported by property.");                                   \
		return ValueType::Type;                                                                    \
	}                                                                                              \
    bool CanGet() const override {                                                                 \
        return true;                                                                               \
    }                                                                                              \
    bool CanSet() const override {                                                                 \
        return true;                                                                               \
    }                                                                                              \
    std::any GetValue(const zaf::Object& object) const override {                                  \
        return dynamic_cast<const Class&>(object).FieldName;                                       \
    }                                                                                              \
    void SetValue(zaf::Object& object, const std::any& value) const override {                     \
        dynamic_cast<Class&>(object).FieldName =                                                   \
            std::any_cast<decltype(reinterpret_cast<Class*>(0)->FieldName)>(value);                \
    }                                                                                              \
};                                                                                                 \
__ZAF_INTERNAL_DEFINE_PROPERTY_VARIABLE(PropertyName)