#pragma once

#include <zaf/object/boxing/boxing_traits.h>
#include <zaf/object/custom_property_value_type.h>

namespace zaf {

template<typename T, typename = void>
struct MakePropertyValueType { };

template<typename T>
struct MakePropertyValueType<T, std::enable_if_t<HasBoxingTraitsV<T>>> {
    using type = typename BoxingTraits<T>::BoxedInstanceType::element_type;
};

template<typename T>
struct MakePropertyValueType<T, std::enable_if_t<HasCustomPropertyValueTypeV<T>>> {
    using type = typename CustomPropertyValueTypeHandler<T>::BoxedType;
};

template<typename T>
using MakePropertyValueTypeT = typename MakePropertyValueType<T>::type;

}