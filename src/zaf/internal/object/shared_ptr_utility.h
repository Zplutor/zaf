#pragma once

#include <memory>
#include <type_traits>

namespace zaf::internal {

template<typename T>
struct IsSharedPtr : std::false_type { };

template<typename E>
struct IsSharedPtr<std::shared_ptr<E>> : std::true_type { };

template<typename T>
constexpr bool IsSharedPtrV = IsSharedPtr<T>::value;


template<typename T>
struct GetSharedPtrElementType { 
    using type = void;
};

template<typename E>
struct GetSharedPtrElementType<std::shared_ptr<E>> {
    using type = E;
};

template<typename T>
using GetSharedPtrElementTypeT = typename GetSharedPtrElementType<T>::type;

}