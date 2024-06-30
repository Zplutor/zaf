#pragma once

#include <zaf/base/as.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/internal/object/property_helper.h>
#include <zaf/object/boxing/boxing.h>
#include <zaf/object/internal/property_registrar.h>
#include <zaf/object/object_property.h>
#include <zaf/object/property_value_traits.h>

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
        static std::shared_ptr<zaf::Object> InnerGet(                                             \
            const T& object,                                                                      \
            GetterDeclaredType<T>* value_type) {                                                  \
            using Traits = zaf::PropertyValueTraits<std::decay_t<decltype(*value_type)>>;         \
            return Traits::ToBoxedObject(object.PropertyName());                                             \
        }                                                                                         \
        template<typename T>                                                                      \
        static std::shared_ptr<zaf::Object> InnerGet(const T& object, ...) {                      \
            throw zaf::InvalidOperationError{ ZAF_SOURCE_LOCATION() };                            \
        }                                                                                         \
        template<typename T>                                                                      \
        static void InnerSet(                                                                     \
            T& object,                                                                            \
            const std::shared_ptr<zaf::Object>& value,                                            \
            SetterDeclaredType<T>* value_type) {                                                  \
            using ValueDeclaredType = std::decay_t<decltype(*value_type)>;                        \
            using Traits = zaf::PropertyValueTraits<ValueDeclaredType>;                           \
            object.Set##PropertyName(Traits::FromBoxedObject(value));                                      \
        }                                                                                         \
        template<typename T>                                                                      \
        static void InnerSet(T& object, const std::shared_ptr<zaf::Object>& value, ...) {         \
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
    public:                                                                                       \
        using DeclaredType = std::remove_pointer_t<decltype(DeduceDeclaredType<Class>(nullptr))>; \
        using ValueType = typename zaf::PropertyValueTraits<DeclaredType>::BoxedType;             \
        static constexpr bool CanGet = InnerCanGet<Class>(nullptr);                               \
        static constexpr bool CanSet = InnerCanSet<Class>(nullptr);                               \
        static std::shared_ptr<zaf::Object> Get(const zaf::Object& object) {                      \
            return InnerGet(zaf::As<Class>(object), nullptr);                                     \
        }                                                                                         \
        static void Set(zaf::Object& object, const std::shared_ptr<zaf::Object>& value) {         \
            InnerSet(zaf::As<Class>(object), value, nullptr);                                     \
        }                                                                                         \
    };                                                                                            \
    class PropertyName##PropertyType : public zaf::ObjectProperty {                               \
    public:                                                                                       \
        const std::wstring& GetName() const override {                                            \
            static const std::wstring& name{ L#PropertyName };                                    \
            return name;                                                                          \
        }                                                                                         \
        zaf::ObjectType* GetValueType() const override {                                          \
            return PropertyName##Traits::ValueType::StaticType();                                 \
        }                                                                                         \
        bool IsValueTypeDynamic() const override {                                                \
            return zaf::internal::IsSharedPtrV<PropertyName##Traits::DeclaredType>;               \
        }                                                                                         \
        bool CanGet() const override {                                                            \
            return PropertyName##Traits::CanGet;                                                  \
        }                                                                                         \
        bool CanSet() const override {                                                            \
            return PropertyName##Traits::CanSet;                                                  \
        }                                                                                         \
        std::shared_ptr<zaf::Object> GetValue(const zaf::Object& object) const override {         \
            return PropertyName##Traits::Get(object);                                             \
        }                                                                                         \
        void SetValue(                                                                            \
            zaf::Object& object,                                                                  \
            const std::shared_ptr<zaf::Object>& value) const override {                           \
            return PropertyName##Traits::Set(object, value);                                      \
        }                                                                                         \
    };                                                                                            \
public:                                                                                           \
    zaf::ObjectProperty* const PropertyName##Property = []() {                                    \
        static PropertyName##PropertyType property;                                               \
        zaf::internal::PropertyRegistrar::Register(Class::StaticType(), &property);               \
        return &property;                                                                         \
    }();