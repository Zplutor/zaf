#pragma once

#include <type_traits>
#include <zaf/object/object.h>

namespace zaf {

template<typename T>
struct CustomPropertyValueTypeHandler {
    //BoxedType
    //Box
    //Unbox
};


template<typename T, typename = void>
struct HasCustomPropertyValueType : std::false_type { };

template<typename T>
struct HasCustomPropertyValueType<T,
    std::void_t<typename CustomPropertyValueTypeHandler<T>::BoxedType>> :
    std::true_type { };

template<typename T>
constexpr bool HasCustomPropertyValueTypeV = HasCustomPropertyValueType<T>::value;

}