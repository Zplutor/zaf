#pragma once

#include <type_traits>
#include <zaf/object/boxing/boolean.h>
#include <zaf/object/boxing/boxed_object.h>
#include <zaf/object/boxing/internal/built_in_box_types.h>
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


template<typename T>
struct ToBoxedTypeImpl {
private:
    using DecayType = std::decay_t<T>;

    static_assert(!IsSharedPtrImpl<DecayType>::Value, "T cannot be a std::shared_ptr.");

public:
    using Type = std::conditional_t<
        IsReflectiveType<DecayType>,
        DecayType,
        std::conditional_t<
            HasBuiltInBoxType<T>::Value,
            typename GetBuiltInBoxType<T>::Type,
            BoxedObject<T>
        >
    >;
};


template<typename T>
struct ToBoxedInstanceTypeImpl {
private:
    using DecayType = std::decay_t<T>;

    struct SharedPtrConverter {
        using Type = typename ToBoxedTypeImpl<
            typename GetSharedPtrElementTypeImpl<DecayType>::Type
        >::Type;
    };

    struct NonSharedPtrConverter {
        using Type = typename ToBoxedTypeImpl<DecayType>::Type;
    };

    using Converter = std::conditional_t<
        IsSharedPtrImpl<DecayType>::Value, 
        SharedPtrConverter,
        NonSharedPtrConverter
    >;

    using BoxedType = typename Converter::Type;

public:
    using Type = std::shared_ptr<BoxedType>;
};

}

/**
Converts the specified type to a corresponding boxed type, which is a reflective type.

@param T
    The type to convert. It cannot be a std::shared_ptr.

@return
    The corresponding boxed type, upon which zaf::IsReflectiveType<> will return true.
    If T is already a boxed type, the return type is the same as T.
*/
template<typename T>
using ToBoxedType = typename internal::ToBoxedTypeImpl<T>::Type;


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
using ToBoxedInstanceType = typename internal::ToBoxedInstanceTypeImpl<T>::Type;

}