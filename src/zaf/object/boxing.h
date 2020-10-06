#pragma once

#include <zaf/base/error/check.h>
#include <zaf/object/internal/get_box_type.h>
#include <zaf/object/internal/unboxer.h>

namespace zaf {

template<typename T>
std::shared_ptr<typename internal::GetBoxType<T>::Type> Box(const T& value) {
    return std::make_shared<typename internal::GetBoxType<T>::Type>(value);
}


template<typename T>
const T* TryUnbox(const Object& object) {
    return internal::GetUnboxer<T>::Type::Unbox(object);
}


template<typename T>
const T* TryUnbox(const std::shared_ptr<Object>& object) {

    ZAF_EXPECT(object);
    return TryUnbox<T>(*object);
}

}