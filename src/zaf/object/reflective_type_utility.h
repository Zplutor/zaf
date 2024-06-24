#pragma once

#include <type_traits>
#include <zaf/object/object.h>

namespace zaf {

template<typename T>
struct IsReflectiveType {
    static constexpr bool value = std::is_same_v<Object, T> || std::is_base_of_v<Object, T>;
};


/**
Indicates whether the specified type is a reflective type, that is, it is either zaf::Object or a
derived type of zaf::Object.

@param T
    The type to check.

@return
    true if T is a reflective type; otherwise false.
*/
template<typename T>
constexpr bool IsReflectiveTypeV = IsReflectiveType<T>::value;

}