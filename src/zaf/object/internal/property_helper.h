#pragma once

#include <type_traits>
#include <zaf/object/boxing/boxing.h>
#include <zaf/object/boxing/internal/get_box_type.h>
#include <zaf/object/object.h>

namespace zaf::internal {

template<typename T>
struct DeduceGetterType { };

template<typename T, typename R>
struct DeduceGetterType<R(T::*)()const> {
    using Type = std::decay_t<R>;
};


template<typename T>
struct DeduceSetterType { };

template<typename T, typename A>
struct DeduceSetterType<void(T::*)(A)> {
    using Type = std::decay_t<A>;
};


template<typename T>
struct GetSharedPtrElementType {
    using Type = void;
};

template<typename E>
struct GetSharedPtrElementType<std::shared_ptr<E>> {
    using Type = E;
};

template<typename T>
struct IsBoxedType {
    static constexpr bool Value = 
        std::is_base_of_v<zaf::Object, typename GetSharedPtrElementType<T>::Type>;
};

template<typename T>
struct DeduceUnderlyingValueType {

    using Type = std::conditional_t<
        IsBoxedType<T>::Value,
        typename GetSharedPtrElementType<T>::Type,
        typename zaf::internal::GetBoxType<T>::Type
    >;
};


template<typename T>
struct IsReflectionType {
    static constexpr bool Value = &T::Type != &zaf::Object::Type;
};


template<typename T>
struct BoxedTypeBoxer {
    static T Box(const T& value) {
        return value;
    }
};

template<typename T>
struct NonBoxedTypeBoxer {
    static std::shared_ptr<zaf::Object> Box(const T& value) {
        return zaf::Box(value);
    }
};

template<typename T>
std::shared_ptr<Object> BoxPropertyValue(const T& value) {
    using Type = std::conditional_t<IsBoxedType<T>::Value, BoxedTypeBoxer<T>, NonBoxedTypeBoxer<T>>;
    return Type::Box(value);
}


template<typename T>
struct BoxedTypeUnboxer {
    static T Unbox(const std::shared_ptr<Object>& value) {
        return std::dynamic_pointer_cast<T::element_type>(value);
    }
};

template<typename T>
struct NonBoxedTypeUnboxer {
    static const T& Unbox(const std::shared_ptr<Object>& value) {
        return zaf::Unbox<T>(value);
    }
};

template<typename T>
struct GetPropertyUnboxer {
    using Type = std::conditional_t<
        IsBoxedType<T>::Value,
        BoxedTypeUnboxer<T>,
        NonBoxedTypeUnboxer<T>
    >;
};

}