#pragma once

#include <type_traits>
#include <zaf/object/boxing/boolean.h>
#include <zaf/object/boxing/custom_boxed_type_utility.h>
#include <zaf/object/boxing/numeric.h>
#include <zaf/object/boxing/string.h>
#include <zaf/object/reflective_type_utility.h>

namespace zaf {
namespace internal {

template<typename T>
struct IsSharedPtrImpl {
    static constexpr bool Value = false;
};

template<typename E>
struct IsSharedPtrImpl<std::shared_ptr<E>> {
    static constexpr bool Value = true;
};

template<typename T>
struct GetSharedPtrElementTypeImpl {
    using Type = void;
};

template<typename E>
struct GetSharedPtrElementTypeImpl<std::shared_ptr<E>> {
    using Type = E;
};

}


template<typename T>
struct ToBoxedType {
private:
    using DecayType = std::decay_t<T>;

public:
    using type = std::conditional_t<
        IsReflectiveTypeV<DecayType>,
        DecayType,
        GetCustomBoxedTypeT<DecayType>
    >;
};

/**
Converts the specified type to a corresponding boxed type, which is a reflective type.

@param T
    The type to convert. It cannot be a std::shared_ptr.

@return
    The corresponding boxed type, upon which zaf::IsReflectiveType<> will return true.
    If T is already a boxed type, the return type is the same as T.
*/
template<typename T>
using ToBoxedTypeT = typename ToBoxedType<T>::type;


template<typename T>
struct ToBoxedInstanceType {
private:
    using DecayType = std::decay_t<T>;

    struct SharedPtrConverter {
        using Type = ToBoxedTypeT<
            typename internal::GetSharedPtrElementTypeImpl<DecayType>::Type
        >;
    };

    struct NonSharedPtrConverter {
        using Type = ToBoxedTypeT<DecayType>;
    };

    using Converter = std::conditional_t<
        internal::IsSharedPtrImpl<DecayType>::Value,
        SharedPtrConverter,
        NonSharedPtrConverter
    >;

    using BoxedType = typename Converter::Type;

public:
    using type = std::shared_ptr<BoxedType>;
};

/**
Converts a specified type to a corresponding boxed instance type, which is a std::shared_ptr to a 
boxed type.

@param T
    The type to convert.

@return
    The corresponding boxed instance type. If T is already a std::shared_ptr, its element_type will
    be converted to a boxed type; otherwise, T will be converted to a boxed type wrapped in 
    std::shared_ptr.
*/
template<typename T>
using ToBoxedInstanceTypeT = typename ToBoxedInstanceType<T>::type;

}