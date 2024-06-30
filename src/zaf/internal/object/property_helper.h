#pragma once

#include <type_traits>

namespace zaf::internal {

template<typename T>
struct DeduceGetterReturnType { };

template<typename T, typename R>
struct DeduceGetterReturnType<R(T::*)()const> {
    using Type = std::decay_t<R>;
};


template<typename T>
struct DeduceSetterArgumentType { };

template<typename T, typename A>
struct DeduceSetterArgumentType<void(T::*)(A)> {
    using Type = std::decay_t<A>;
};

}