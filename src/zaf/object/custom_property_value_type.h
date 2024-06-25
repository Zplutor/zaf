#pragma once

#include <type_traits>

namespace zaf {

template<typename T>
struct GetCustomPropertyValueType { };

template<typename T>
using GetCustomPropertyValueTypeT = typename GetCustomPropertyValueType<T>::type;


template<typename T, typename = void>
struct HasCustomPropertyValueType : std::false_type { };

template<typename T>
struct HasCustomPropertyValueType<T, std::void_t<GetCustomPropertyValueTypeT<T>>> : 
    std::true_type { };

template<typename T>
constexpr bool HasCustomPropertyValueTypeV = HasCustomPropertyValueType<T>::value;

}