#pragma once

#include <zaf/base/error/contract_error.h>
#include <zaf/base/error/invalid_type_error.h>
#include <zaf/object/boxing/boxing_traits.h>
#include <zaf/object/boxing/boxed_type.h>

namespace zaf {

template<typename T>
auto Box(T&& value) {
    return BoxingTraits<T>::Box(std::forward<T>(value));
}


template<typename T>
const T& Unbox(const Object& object) {
    auto unboxed_value = BoxingTraits<T>::Unbox(object);
    if (unboxed_value) {
        return *unboxed_value;
    }
    throw InvalidTypeError{ ZAF_SOURCE_LOCATION() };
}

template<typename T>
T& Unbox(Object& object) {
    auto unboxed_value = BoxingTraits<T>::Unbox(object);
    if (unboxed_value) {
        return const_cast<T&>(*unboxed_value);
    }
    throw InvalidTypeError{ ZAF_SOURCE_LOCATION() };
}


template<typename T>
const T* Unbox(const Object* object) {
    ZAF_EXPECT(object);
    return BoxingTraits<T>::Unbox(*object);
}

template<typename T>
T* Unbox(Object* object) {
    ZAF_EXPECT(object);
    return const_cast<T*>(BoxingTraits<T>::Unbox(*object));
}


template<typename T>
T* Unbox(const std::shared_ptr<Object>& object) {
    return Unbox<T>(object.get());
}

}