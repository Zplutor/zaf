#pragma once

#include <zaf/base/as.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/base/non_copyable.h>
#include <zaf/object/boxing/boxing.h>
#include <zaf/object/internal/property_helper.h>
#include <zaf/object/internal/property_registrar.h>
#include <zaf/object/object_property.h>

#define ZAF_DECLARE_PROPERTY \
class _PropertyMeta; \
static _PropertyMeta& PropertyMeta();

#define ZAF_PROPERTY_BEGIN(ClassName)                                                             \
class ClassName::_PropertyMeta : zaf::NonCopyableNonMovable {                                                \
private:                                                                                          \
    using Class = ClassName;                                                                      \
    static _PropertyMeta instance;                                                                \
    _PropertyMeta() = default;                                                                    \
public:                                                                                           \
    static _PropertyMeta& Instance() { return instance; }


#define ZAF_PROPERTY(PropertyName)                                                                \
private:                                                                                          \
    struct PropertyName##Traits {                                                                 \
        template<typename T>                                                                      \
        using GetterDeclaredType =                                                                \
            typename zaf::internal::DeduceGetterType<decltype(&T::PropertyName)>::Type;           \
        template<typename T>                                                                      \
        using SetterDeclaredType =                                                                \
            typename zaf::internal::DeduceSetterType<decltype(&T::Set##PropertyName)>::Type;      \
        template<typename T>                                                                      \
        static constexpr bool InnerCanGet(GetterDeclaredType<T>*) { return true; }                \
        template<typename T>                                                                      \
        static constexpr bool InnerCanGet(...) { return false; }                                  \
        template<typename T>                                                                      \
        static constexpr bool InnerCanSet(SetterDeclaredType<T>*) { return true ;}                \
        template<typename T>                                                                      \
        static constexpr bool InnerCanSet(...) { return false; }                                  \
        template<typename T>                                                                      \
        static std::shared_ptr<zaf::Object> InnerGet(const T& object, GetterDeclaredType<T>*) {   \
            return zaf::internal::BoxPropertyValue(object.PropertyName());                        \
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
            using Unboxer = typename zaf::internal::GetPropertyUnboxer<                           \
                std::decay_t<decltype(*value_type)>>::Type;                                       \
            object.Set##PropertyName(Unboxer::Unbox(value));                                      \
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
        using BoxedType = zaf::internal::DeduceUnderlyingValueType<DeclaredType>::Type;           \
        static_assert(zaf::internal::IsReflectionType<BoxedType>::Value,                          \
            "This type of value is not supported by property.");                                  \
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
            return PropertyName##Traits::BoxedType::Type;                                         \
        }                                                                                         \
        bool IsValueTypeDynamic() const override {                                                \
            return zaf::internal::IsSharedPtr<PropertyName##Traits::DeclaredType>::Value;         \
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
    zaf::ObjectProperty* const PropertyName = []() {                                              \
        static PropertyName##PropertyType property;                                               \
        zaf::internal::PropertyRegistrar::Register(Class::Type, &property);                       \
        return &property;                                                                         \
    }();


#define ZAF_PROPERTY_END };


#define ZAF_DEFINE_PROPERTY(ClassName) \
ClassName::_PropertyMeta ClassName::_PropertyMeta::instance;\
ClassName::_PropertyMeta& ClassName::PropertyMeta() { \
    return ClassName::_PropertyMeta::Instance(); \
};
