#pragma once

#include <zaf/base/error/contract_error.h>
#include <zaf/object/boxing/boxed_type.h>
#include <zaf/object/boxing/internal/unboxer.h>

namespace zaf {

template<typename T>
auto Box(T&& value) {
    using BoxedInstanceType = ToBoxedInstanceTypeT<T>;
    return std::make_shared<typename BoxedInstanceType::element_type>(std::forward<T>(value));
}


template<typename T>
const T& Unbox(const Object& object) {
    return internal::GetUnboxer<T>::Type::Unbox(object);
}

template<typename T>
T& Unbox(Object& object) {
    return internal::GetUnboxer<T>::Type::Unbox(object);
}


template<typename T>
const T* Unbox(const Object* object) {
    ZAF_EXPECT(object);
    return internal::GetUnboxer<T>::Type::TryUnbox(*object);
}

template<typename T>
T* Unbox(Object* object) {
    ZAF_EXPECT(object);
    return internal::GetUnboxer<T>::Type::TryUnbox(*object);
}


template<typename T>
T* Unbox(const std::shared_ptr<Object>& object) {
    ZAF_EXPECT(object);
    return Unbox<T>(object.get());
}

}