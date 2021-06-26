#pragma once

#include <type_traits>

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

}