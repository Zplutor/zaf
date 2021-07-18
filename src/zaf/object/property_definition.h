#pragma once

#include <zaf/object/boxing/boxing.h>
#include <zaf/object/internal/property_helper.h>
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


#define ZAF_DEFINE_TYPE_PROPERTY(PropertyName)                                                     \
struct PropertyName##Accessor {                                                                    \
    template<typename T>                                                                           \
    using GetterValueType =                                                                        \
        typename zaf::internal::DeduceGetterType<decltype(&T::PropertyName)>::Type;                \
    template<typename T>                                                                           \
    using SetterValueType =                                                                        \
        typename zaf::internal::DeduceSetterType<decltype(&T::Set##PropertyName)>::Type;           \
    template<typename T>                                                                           \
    static constexpr bool InnerCanGet(GetterValueType<T>*) {                                       \
        return true;                                                                               \
    }                                                                                              \
    template<typename T>                                                                           \
    static constexpr bool InnerCanGet(...) {                                                       \
        return false;                                                                              \
    }                                                                                              \
    template<typename T>                                                                           \
    static constexpr bool InnerCanSet(SetterValueType<T>*) {                                       \
        return true;                                                                               \
    }                                                                                              \
    template<typename T>                                                                           \
    static constexpr bool InnerCanSet(...) {                                                       \
        return false;                                                                              \
    }                                                                                              \
    template<typename T>                                                                           \
    static std::shared_ptr<zaf::Object> InnerGet(const T& object, GetterValueType<T>*) {           \
        return zaf::internal::BoxPropertyValue(object.PropertyName());                             \
    }                                                                                              \
    template<typename T>                                                                           \
    static std::shared_ptr<zaf::Object> InnerGet(const T& object, ...) {                           \
        throw std::exception{};                                                                    \
    }                                                                                              \
    template<typename T>                                                                           \
    static void InnerSet(                                                                          \
        T& object,                                                                                 \
        const std::shared_ptr<zaf::Object>& value,                                                 \
        SetterValueType<T>* value_type) {                                                          \
        using Unboxer =                                                                            \
            zaf::internal::GetPropertyUnboxer<std::decay_t<decltype(*value_type)>>::Type;          \
        object.Set##PropertyName(Unboxer::Unbox(value));                                           \
    }                                                                                              \
    template<typename T>                                                                           \
    static void InnerSet(T& object, const std::shared_ptr<zaf::Object>& value, ...) {              \
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
public:                                                                                            \
    using ValueType = zaf::internal::DeduceUnderlyingValueType<                                    \
        std::remove_pointer_t<decltype(DeduceValueType<Class>(nullptr))>                           \
    >::Type;                                                                                       \
    static_assert(zaf::internal::IsReflectionType<ValueType>::Value,                               \
        "This type of value is not supported by property.");                                       \
    static constexpr bool CanGet = InnerCanGet<Class>(nullptr);                                    \
    static constexpr bool CanSet = InnerCanSet<Class>(nullptr);                                    \
    static std::shared_ptr<zaf::Object> Get(const zaf::Object& object) {                           \
        return InnerGet(dynamic_cast<const Class&>(object), nullptr);                              \
    }                                                                                              \
    static void Set(zaf::Object& object, const std::shared_ptr<zaf::Object>& value) {              \
        InnerSet(dynamic_cast<Class&>(object), value, nullptr);                                    \
    }                                                                                              \
};                                                                                                 \
class PropertyName##Property : public zaf::ObjectProperty {                                        \
public:                                                                                            \
    const std::wstring& GetName() const override {                                                 \
        static const std::wstring name{ L#PropertyName };                                          \
        return name;                                                                               \
    }                                                                                              \
    ObjectType* GetValueType() const override {                                                    \
        return PropertyName##Accessor::ValueType::Type;                                            \
    }                                                                                              \
    bool CanGet() const override {                                                                 \
        return PropertyName##Accessor::CanGet;                                                     \
    }                                                                                              \
    bool CanSet() const override {                                                                 \
        return PropertyName##Accessor::CanSet;                                                     \
    }                                                                                              \
    std::shared_ptr<zaf::Object> GetValue(const zaf::Object& object) const override {              \
        return PropertyName##Accessor::Get(object);                                                \
    }                                                                                              \
    void SetValue(zaf::Object& object, const std::shared_ptr<zaf::Object>& value) const override { \
        return PropertyName##Accessor::Set(object, value);                                         \
    }                                                                                              \
};                                                                                                 \
__ZAF_INTERNAL_DEFINE_PROPERTY_VARIABLE(PropertyName)


#define ZAF_DEFINE_TYPE_PROPERTY_WITH_FIELD(PropertyName, FieldName)                               \
class PropertyName##Property : public zaf::ObjectProperty {                                        \
public:                                                                                            \
    const std::wstring& GetName() const override {                                                 \
        static const std::wstring name{ L#PropertyName };                                          \
        return name;                                                                               \
    }                                                                                              \
    ObjectType* GetValueType() const override {                                                    \
        using ValueType = zaf::internal::DeduceUnderlyingValueType<                                \
            decltype(reinterpret_cast<Class*>(0)->FieldName)                                       \
        >::Type;                                                                                   \
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
    std::shared_ptr<zaf::Object> GetValue(const zaf::Object& object) const override {              \
        return zaf::internal::BoxPropertyValue(dynamic_cast<const Class&>(object).FieldName);      \
    }                                                                                              \
    void SetValue(zaf::Object& object, const std::shared_ptr<zaf::Object>& value) const override { \
        using Unboxer = zaf::internal::GetPropertyUnboxer<                                         \
            decltype(reinterpret_cast<Class*>(0)->FieldName)                                       \
        >::Type;                                                                                   \
        dynamic_cast<Class&>(object).FieldName = Unboxer::Unbox(value);                            \
    }                                                                                              \
};                                                                                                 \
__ZAF_INTERNAL_DEFINE_PROPERTY_VARIABLE(PropertyName)