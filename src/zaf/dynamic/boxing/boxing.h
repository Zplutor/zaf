#pragma once

/**
@file
    Defines functions relate to boxing and unboxing.
*/

#include <zaf/base/error/invalid_type_error.h>
#include <zaf/dynamic/boxing/boxing_traits.h>

namespace zaf {

/**
Boxes the specified non-boxed value to a boxed object.

@tparam T
    The type of the non-boxed value. If it does not support boxing, a compile error will be 
    emitted.

@param value
    The non-boxed value to box. It should be copyable or movable.

@return
    The boxed object, which is a `std::shared_ptr` to the boxed type for `T`.

@post
    The returned value is not null.

@throw std::bad_alloc
    Thrown if fails to allocate the memory for the boxed object.

@throw ...
    Any exception that thrown by:
    - the copy of `value`;
    - the custom boxing operation in zaf__CustomBoxingTraits.

@details
    This function uses zaf::BoxingTraits to perform the boxing.
*/
template<typename T>
auto Box(T&& value) {
    return BoxingTraits<T>::Box(std::forward<T>(value));
}


/**
Unboxes the specified object to a non-boxed value.

@tparam T
    The non-boxed type to which the object will be unboxed. If it does not support unboxing, a 
    compile error will be emitted.

@param object
    The object to unbox.

@return
    The unboxed value. It is a reference to the inner storage of the boxed object.

@throw zaf::InvalidTypeError
    Thrown if unboxing fails. It is typically due to `object` is not an instance of the boxed type
    for `T`.

@details
    This function uses zaf::BoxingTraits to perform the unboxing.
*/
template<typename T>
const T& Unbox(const dynamic::Object& object) {
    auto unboxed_value = BoxingTraits<T>::Unbox(object);
    if (unboxed_value) {
        return *unboxed_value;
    }
    throw InvalidTypeError{ ZAF_SOURCE_LOCATION() };
}


/**
Unboxes the specified object to a non-boxed value.

@tparam T
    The non-boxed type to which the object will be unboxed. If it does not support unboxing, a
    compile error will be emitted.

@param object
    The object to unbox.

@return
    The unboxed value. It is a reference to the inner storage of the boxed object.

@throw zaf::InvalidTypeError
    Thrown if unboxing fails. It is typically due to `object` is not an instance of the boxed type
    for `T`.

@details
    This function uses zaf::BoxingTraits to perform the unboxing.
*/
template<typename T>
T& Unbox(dynamic::Object& object) {
    auto unboxed_value = BoxingTraits<T>::Unbox(object);
    if (unboxed_value) {
        return const_cast<T&>(*unboxed_value);
    }
    throw InvalidTypeError{ ZAF_SOURCE_LOCATION() };
}


/**
Unboxes the specified object to a non-boxed value.

@tparam T
    The non-boxed type to which the object will be unboxed. If it does not support unboxing, a
    compile error will be emitted.

@param object
    A pointer to the object to unbox.

@return
    A pointer to the unboxed value. The pointer points to the inner storage of the boxed object. It
    will be null if:
    - `object` is null;
    - the unboxing fails. It is typically due to `object` is not an instance of the boxed type
      for `T`.

@details
    This function uses zaf::BoxingTraits to perform the unboxing.
*/
template<typename T>
const T* Unbox(const dynamic::Object* object) noexcept {
    if (object) {
        return BoxingTraits<T>::Unbox(*object);
    }
    return nullptr;
}


/**
Unboxes the specified object to a non-boxed value.

@tparam T
    The non-boxed type to which the object will be unboxed. If it does not support unboxing, a
    compile error will be emitted.

@param object
    A pointer to the object to unbox.

@return
    A pointer to the unboxed value. The pointer points to the inner storage of the boxed object. It
    will be null if:
    - `object` is null;
    - the unboxing fails. It is typically due to `object` is not an instance of the boxed type
      for `T`.

@details
    This function uses zaf::BoxingTraits to perform the unboxing.
*/
template<typename T>
T* Unbox(dynamic::Object* object) noexcept {
    if (object) {
        return const_cast<T*>(BoxingTraits<T>::Unbox(*object));
    }
    return nullptr;
}


/**
Unboxes the specified object to a non-boxed value.

@tparam T
    The non-boxed type to which the object will be unboxed. If it does not support unboxing, a
    compile error will be emitted.

@param object
    A pointer to the object to unbox.

@return
    A pointer to the unboxed value. The pointer points to the inner storage of the boxed object. It
    will be null if:
    - `object` is null;
    - the unboxing fails. It is typically due to `object` is not an instance of the boxed type
      for `T`.

@details
    This function uses zaf::BoxingTraits to perform the unboxing.
*/
template<typename T>
T* Unbox(const std::shared_ptr<dynamic::Object>& object) noexcept {
    return Unbox<T>(object.get());
}

}