#pragma once

#include <zaf/base/non_copyable.h>

#define ZAF_PROPERTY_BEGIN(ClassName) \
class PropertyMeta : zaf::NonCopyableNonMovable {\
private:\
    using Class = ClassName;\
    static PropertyMeta instance;\
    PropertyMeta() = default;\
public:\
    static PropertyMeta& Instance() { return instance; }


#define ZAF_PROPERTY(PropertyName) \
private:\
    struct PropertyName##PropertyTraits { \
    private: \
        template<typename T> \
        static constexpr auto TestGetterMethodType(T*) -> decltype(&T::PropertyName); \
        template<typename T> \
        static constexpr void TestGetterMethodType(...); \
        /*The type of the getter method*/ \
        using GetterMethodType = decltype(TestGetterMethodType<Class>(nullptr)); \
        /*The type of the argument of the getter method*/\
        using GetterReturnType = \
            typename zaf::internal::DeduceGetterReturnType<GetterMethodType>::Type;\
        /*The actual value type of the getter*/\
        using GetterValueType = std::conditional_t<\
            std::is_void_v<GetterReturnType>, \
            void,  \
            zaf::internal::DeduceUnderlyingValueType<GetterReturnType>::Type\
        >;\
        template<typename T>\
        static constexpr auto TestSetterMethodType(T*) -> decltype(&T::Set##PropertyName);\
        template<typename T>\
        static constexpr void TestSetterMethodType(...);\
        /*The type of the setter method*/ \
        using SetterMethodType = decltype(TestSetterMethodType<Class>(nullptr)); \
        /*The type of the return value of the setter method*/\
        using SetterArgumentType = typename zaf::internal::DeduceSetterArgumentType<SetterMethodType>::Type;\
        /*The actual value type of the setter*/\
        using SetterValueType = std::conditional_t<\
            std::is_void_v<SetterArgumentType>, \
            void,  \
            zaf::internal::DeduceUnderlyingValueType<SetterArgumentType>::Type\
        >;\
        static_assert(std::is_same_v<std::true_type, std::conditional_t< \
            std::is_void_v<GetterValueType> && std::is_void_v<SetterValueType>,\
            std::false_type,\
            std::conditional_t<\
                std::is_void_v<GetterValueType> || std::is_void_v<SetterValueType>,\
                std::true_type,\
                std::conditional_t<\
                    std::is_same_v<GetterValueType, SetterValueType>,\
                    std::true_type,\
                    std::false_type\
                >\
            >\
        >>, "invalid property");\
    public: \
        using SetterRawType = SetterArgumentType;\
        using ValueType = std::conditional_t< \
            !std::is_void_v<GetterValueType>, \
            GetterValueType, \
            SetterValueType \
        >; \
        static constexpr bool CanGet = !std::is_void_v<GetterValueType>; \
        static constexpr bool CanSet = !std::is_void_v<SetterValueType>; \
    };\
    class PropertyName##PropertyType : public zaf::ObjectProperty {\
    private:\
        template<typename T = Class> \
        std::shared_ptr<zaf::Object> GetValueImpl(const zaf::Object& object, std::false_type) const {\
            throw zaf::InvalidOperationError{};\
        }\
        template<typename T = Class> \
        std::shared_ptr<zaf::Object> GetValueImpl(const zaf::Object& object, std::true_type) const {\
            return zaf::internal::BoxPropertyValue(zaf::As<Class>(object).PropertyName());\
        }\
        template<typename T = Class> \
        void SetValueImpl(zaf::Object& object, const std::shared_ptr<zaf::Object>& value, std::false_type) const {\
            throw zaf::InvalidOperationError{};\
        }\
        template<typename T = Class> \
        void SetValueImpl(zaf::Object& object, const std::shared_ptr<zaf::Object>& value, std::true_type) const {\
            using Unboxer = typename zaf::internal::GetPropertyUnboxer<std::decay_t<Traits::SetterRawType>>::Type;     \
            zaf::As<Class>(object).Set##PropertyName(Unboxer::Unbox(value));                  \
        }\
    public:\
        using Traits = PropertyName##PropertyTraits; \
        const std::wstring& GetName() const override {\
            static const std::wstring& name{ L#PropertyName };\
            return name;\
        }\
        zaf::ObjectType* GetValueType() const override {\
            return Traits::ValueType::Type;\
        }\
        bool IsValueTypeDynamic() const override {return false;}\
        bool CanGet() const override {\
            return Traits::CanGet;\
        }\
        bool CanSet() const override {\
            return Traits::CanSet;\
        }\
        std::shared_ptr<zaf::Object> GetValue(const zaf::Object& object) const override {\
            return GetValueImpl(object, std::integral_constant<bool, Traits::CanGet>{});\
        }\
        void SetValue(\
            zaf::Object& object, \
            const std::shared_ptr<zaf::Object>& value) const override { \
            SetValueImpl(object, value, std::integral_constant<bool, Traits::CanSet>{});\
        }\
    };\
public:                                                                                           \
    zaf::ObjectProperty* const PropertyName = []() {                                         \
        static PropertyName##PropertyType property;                                               \
        zaf::internal::PropertyRegistrar::Register(Class::Type, &property);                              \
        return &property;                                                                         \
    }();


#define ZAF_PROPERTY_END \
};\
static PropertyMeta& Properties() {\
    return PropertyMeta::Instance();\
}


#define ZAF_PROPERTY_IMPL(ClassName) \
ClassName::PropertyMeta ClassName::PropertyMeta::instance;
