#pragma once

#include <type_traits>
#include <zaf/object/boxing/boxing.h>
#include <zaf/object/custom_property_value_type.h>
#include <zaf/object/object.h>

namespace zaf::internal {

template<typename T>
struct DeduceGetterReturnType { };

template<typename T, typename R>
struct DeduceGetterReturnType<R(T::*)()const> {
    using Type = std::decay_t<R>;
};


template<typename T>
struct DeduceSetterArgumentType { };

template<typename T, typename A>
struct DeduceSetterArgumentType<void(T::*)(A)> {
    using Type = std::decay_t<A>;
};


template<typename T, typename = void>
struct PropertyValueBoxer { };

template<typename T>
struct PropertyValueBoxer<T, std::enable_if_t<internal::IsSharedPtrV<T>>> {
    static std::shared_ptr<Object> Box(const T& value) {
        return value;
    }
};

template<typename T>
struct PropertyValueBoxer<T, 
    std::enable_if_t<!internal::IsSharedPtrV<T> && HasBoxedInstanceTypeV<T>>> {
    static std::shared_ptr<Object> Box(const T& value) {
        return zaf::Box(value);
    }
};

template<typename T>
struct PropertyValueBoxer<T, std::enable_if_t<HasCustomPropertyValueTypeV<T>>> {
    static std::shared_ptr<Object> Box(const T& value) {
        return BoxCustomPropertyValue(value);
    }
};


template<typename T, typename = void>
struct PropertyValueUnboxer { };

template<typename T>
struct PropertyValueUnboxer<T, std::enable_if_t<internal::IsSharedPtrV<T>>> {
    static T Unbox(const std::shared_ptr<Object>& value) {
        return std::dynamic_pointer_cast<T::element_type>(value);
    }
};

template<typename T>
struct PropertyValueUnboxer<T, 
    std::enable_if_t<!internal::IsSharedPtrV<T> && HasBoxedInstanceTypeV<T>>> {
    static const T& Unbox(const std::shared_ptr<Object>& value) {
        return zaf::Unbox<T>(*value);
    }
};

template<typename T>
struct PropertyValueUnboxer<T, std::enable_if_t<HasCustomPropertyValueTypeV<T>>> {
    static const T& Unbox(const std::shared_ptr<Object>& value) {
        return UnboxCustomPropertyValue<T>(value);
    }
};

}