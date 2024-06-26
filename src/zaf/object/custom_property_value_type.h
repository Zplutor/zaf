#pragma once

#include <type_traits>
#include <zaf/object/object.h>

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


template<typename T>
std::shared_ptr<Object> BoxCustomPropertyValue(T&&);

template<typename T>
const T& UnboxCustomPropertyValue(const std::shared_ptr<Object>&);

}