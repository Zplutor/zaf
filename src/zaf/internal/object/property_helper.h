#pragma once

#include <type_traits>
#include <zaf/object/boxing/boxing.h>
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


template<typename T>
struct GetSharedPtrElementType {
    using Type = void;
};

template<typename E>
struct GetSharedPtrElementType<std::shared_ptr<E>> {
    using Type = E;
    //TODO
    /*
    using Type = std::conditional_t<
        std::is_same_v<E, zaf::Image>,
        zaf::URIImage, //Use URIImage to handle parsing things of Image.
        E
    >;
    */
};

template<typename T>
struct IsBoxedType {
    static constexpr bool Value =
        std::is_base_of_v<zaf::Object, typename GetSharedPtrElementType<T>::Type>;
};

/**
Converts the specified declared type to a boxed type.
*/
template<typename T>
struct GetBoxedTypeImpl {
    using Type = std::conditional_t<
        IsBoxedType<T>::Value,
        typename GetSharedPtrElementType<T>::Type,
        typename zaf::internal::GetBoxType<T>::Type
    >;
};

template<typename T>
using GetBoxedType = typename GetBoxedTypeImpl<T>::Type;


template<typename T>
struct BoxedTypeBoxer {
    static std::shared_ptr<Object> Box(const T& value) {
        //Sometimes T::element_type is not a derived class of Object(such as Image),
        //but the runtime type of value is(such as URIImage),
        //so we use a dynamic cast here.
        return std::dynamic_pointer_cast<Object>(value);
    }
};

template<typename T>
struct NonBoxedTypeBoxer {
    static std::shared_ptr<zaf::Object> Box(const T& value) {
        return zaf::Box(value);
    }
};

template<typename T>
std::shared_ptr<Object> BoxPropertyValue(const T& value) {
    using Type = std::conditional_t<IsBoxedType<T>::Value, BoxedTypeBoxer<T>, NonBoxedTypeBoxer<T>>;
    return Type::Box(value);
}


template<typename T>
struct BoxedTypeUnboxer {
    static T Unbox(const std::shared_ptr<Object>& value) {
        return std::dynamic_pointer_cast<T::element_type>(value);
    }
};

template<typename T>
struct NonBoxedTypeUnboxer {
    static const T& Unbox(const std::shared_ptr<Object>& value) {
        return zaf::Unbox<T>(*value);
    }
};

template<typename T>
struct GetPropertyUnboxer {
    using Type = std::conditional_t<
        IsBoxedType<T>::Value,
        BoxedTypeUnboxer<T>,
        NonBoxedTypeUnboxer<T>
    >;
};

}