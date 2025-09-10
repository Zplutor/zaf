#pragma once

#include <zaf/base/as.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/internal/object/property_helper.h>
#include <zaf/dynamic/boxing/boxing.h>
#include <zaf/dynamic/internal/property_registrar.h>
#include <zaf/dynamic/object_property.h>

#define ZAF_OBJECT_PROPERTY(PropertyName)                                                         \
private:                                                                                          \
    struct PropertyName##Traits {                                                                 \
        template<typename T>                                                                      \
        using GetterDeclaredType =                                                                \
            typename zaf::internal::DeduceGetterReturnType<decltype(&T::PropertyName)>::Type;     \
        template<typename T>                                                                      \
        using SetterDeclaredType =                                                                \
            typename zaf::internal::DeduceSetterArgumentType<                                     \
                decltype(&T::Set##PropertyName)>::Type;                                           \
        template<typename T>                                                                      \
        static constexpr bool InnerCanGet(GetterDeclaredType<T>*) { return true; }                \
        template<typename T>                                                                      \
        static constexpr bool InnerCanGet(...) { return false; }                                  \
        template<typename T>                                                                      \
        static constexpr bool InnerCanSet(SetterDeclaredType<T>*) { return true ;}                \
        template<typename T>                                                                      \
        static constexpr bool InnerCanSet(...) { return false; }                                  \
        template<typename T>                                                                      \
        static std::shared_ptr<zaf::dynamic::Object> InnerGet(const T& object, GetterDeclaredType<T>*) {   \
            return zaf::internal::BoxPropertyValue(object.PropertyName());                        \
        }                                                                                         \
        template<typename T>                                                                      \
        static std::shared_ptr<zaf::dynamic::Object> InnerGet(const T& object, ...) {                      \
            throw zaf::InvalidOperationError{ ZAF_SOURCE_LOCATION() };                            \
        }                                                                                         \
        template<typename T>                                                                      \
        static void InnerSet(                                                                     \
            T& object,                                                                            \
            const std::shared_ptr<zaf::dynamic::Object>& value,                                            \
            SetterDeclaredType<T>* value_type) {                                                  \
            using ValueDeclaredType = std::decay_t<decltype(*value_type)>;                        \
            using Traits = zaf::PropertyValueTraits<ValueDeclaredType>;                           \
            object.Set##PropertyName(Traits::FromBoxedObject(value));                             \
        }                                                                                         \
        template<typename T>                                                                      \
        static void InnerSet(T& object, const std::shared_ptr<zaf::dynamic::Object>& value, ...) {         \
            throw zaf::InvalidOperationError{ ZAF_SOURCE_LOCATION() };                            \
        }                                                                                         \
        template<typename T>                                                                      \
        static constexpr GetterDeclaredType<T>* DeduceDeclaredType(std::nullptr_t) {              \
            return nullptr;                                                                       \
        }                                                                                         \
        template<typename T>                                                                      \
        static constexpr SetterDeclaredType<T>* DeduceDeclaredType(...) {                         \
            return nullptr;                                                                       \
        }                                                                                         \
        /* A workaround to solve the mis-reported intellisense error */                           \
        template<typename T>                                                                      \
        static zaf::dynamic::ObjectType* GetObjectType() {                                        \
            return T::StaticType();                                                               \
        }                                                                                         \
    public:                                                                                       \
        using DeclaredType = std::remove_pointer_t<decltype(DeduceDeclaredType<Class>(nullptr))>; \
        static zaf::dynamic::ObjectType* GetValueType() {                                         \
            using BoxedType = typename zaf::PropertyValueTraits<DeclaredType>::BoxedType;         \
            return GetObjectType<BoxedType>();                                                    \
        }                                                                                         \
        static constexpr bool CanGet = InnerCanGet<Class>(nullptr);                               \
        static constexpr bool CanSet = InnerCanSet<Class>(nullptr);                               \
        static std::shared_ptr<zaf::dynamic::Object> Get(const zaf::dynamic::Object& object) {                      \
            return InnerGet(zaf::As<Class>(object), nullptr);                                     \
        }                                                                                         \
        static void Set(zaf::dynamic::Object& object, const std::shared_ptr<zaf::dynamic::Object>& value) {         \
            InnerSet(zaf::As<Class>(object), value, nullptr);                                     \
        }                                                                                         \
    };                                                                                            \
    class PropertyName##PropertyType : public zaf::dynamic::ObjectProperty { \
    public:                                                                                       \
        std::wstring_view Name() const noexcept override {                                        \
            return std::wstring_view{ L#PropertyName };                                           \
        }                                                                                         \
        zaf::dynamic::ObjectType* ValueType() const noexcept override {                           \
            return PropertyName##Traits::GetValueType();                                          \
        }                                                                                         \
        bool IsValueDynamic() const noexcept override {                                           \
            return zaf::IsBoxedInstanceTypeV<PropertyName##Traits::DeclaredType>;                 \
        }                                                                                         \
        bool CanGet() const noexcept override {                                                   \
            return PropertyName##Traits::CanGet;                                                  \
        }                                                                                         \
        bool CanSet() const noexcept override {                                                   \
            return PropertyName##Traits::CanSet;                                                  \
        }                                                                                         \
        std::shared_ptr<zaf::dynamic::Object> GetValue(const zaf::dynamic::Object& object) const override {         \
            return PropertyName##Traits::Get(object);                                             \
        }                                                                                         \
        void SetValue(                                                                            \
            zaf::dynamic::Object& object,                                                                  \
            const std::shared_ptr<zaf::dynamic::Object>& value) const override {                           \
            return PropertyName##Traits::Set(object, value);                                      \
        }                                                                                         \
    };                                                                                            \
public:                                                                                           \
    zaf::dynamic::ObjectProperty* const PropertyName##Property = []() { \
        static PropertyName##PropertyType property;                                               \
        zaf::internal::PropertyRegistrar::Register(Class::StaticType(), &property);               \
        return &property;                                                                         \
    }();