#pragma once

/**
@file
    Defines type utilities related to reflective type.
*/

#include <type_traits>
#include <zaf/object/object.h>

namespace zaf {

/**
A type trait that checks whether the specified type is a reflective type.

@tparam T
    The type to be checked. Its decayed type will be used to check.

@details
    A reflective type is either zaf::Object or a derived type of zaf::Object.
*/
template<typename T>
struct IsReflectiveType {
private:
    using DecayedT = std::decay_t<T>;

public:
    /**
    Indicates whether T is a reflective type.
    */
    static constexpr bool value =
        std::is_same_v<Object, DecayedT> ||
        std::is_base_of_v<Object, DecayedT>;
};


/**
A helper variable template for IsReflectiveType.

@relates IsReflectiveType
*/
template<typename T>
constexpr bool IsReflectiveTypeV = IsReflectiveType<T>::value;

}