#pragma once

#include <type_traits>
#include <zaf/dynamic/property_value_traits.h>

namespace zaf::internal {

template<typename T>
struct DeduceGetterReturnType { };

template<typename T, typename R>
struct DeduceGetterReturnType<R(T::*)()const> {
    using Type = std::decay_t<R>;
};

template<typename T, typename R>
struct DeduceGetterReturnType<R(T::*)()const noexcept> {
    using Type = std::decay_t<R>;
};


template<typename T>
struct DeduceSetterArgumentType { };

template<typename T, typename A>
struct DeduceSetterArgumentType<void(T::*)(A)> {
    using Type = std::decay_t<A>;
};

template<typename T, typename A>
struct DeduceSetterArgumentType<void(T::*)(A) noexcept> {
    using Type = std::decay_t<A>;
};


template<typename T>
auto BoxPropertyValue(T&& value) {
    return PropertyValueTraits<T>::ToBoxedObject(std::forward<T>(value));
}

}