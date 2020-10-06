#pragma once

#include <type_traits>
#include <zaf/object/box_represent.h>
#include <zaf/object/internal/built_in_box_types.h>
#include <zaf/object/object.h>

namespace zaf {

template<typename T>
class Boxed : public Object, public BoxRepresent<T> {
public:
    static_assert(
        !internal::HasBuiltInBoxType<T>::Value,
        "Cannot use Boxed with type T which has built-in box type.");

    static_assert(
        !std::is_base_of_v<Object, T>, 
        "Cannot use Boxed with type T which is derived from Object.");

    using BoxRepresent<T>::BoxRepresent;
};

}