#pragma once

#include <zaf/object/boxing/boxed_type.h>
#include <zaf/object/custom_property_value_type.h>

namespace zaf {

template<typename T, typename = void>
struct MakePropertyValueType { };

template<typename T>
struct MakePropertyValueType<T, std::enable_if_t<HasBoxedInstanceTypeV<T>>> {
    using type = typename ToBoxedInstanceTypeT<T>::element_type;
};

template<typename T>
struct MakePropertyValueType<T, std::enable_if_t<HasCustomPropertyValueTypeV<T>>> {
    using type = GetCustomPropertyValueTypeT<T>;
};

template<typename T>
using MakePropertyValueTypeT = typename MakePropertyValueType<T>::type;

}