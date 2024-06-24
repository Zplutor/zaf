#pragma once

#include <type_traits>

namespace zaf {

template<typename T>
struct GetCustomPropertyValueType {

};

template<typename T>
using GetCustomPropertyValueTypeT = typename GetCustomPropertyValueType<T>::type;

}