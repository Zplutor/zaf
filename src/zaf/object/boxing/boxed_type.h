#pragma once

/**
@file
    Defines type utilities related to boxed type.
*/

#include <type_traits>
#include <zaf/internal/object/shared_ptr_utility.h>
#include <zaf/object/boxing/boolean.h>
#include <zaf/object/boxing/custom_boxing_traits.h>
#include <zaf/object/boxing/numeric.h>
#include <zaf/object/boxing/string.h>
#include <zaf/object/reflective_type.h>

namespace zaf {
namespace internal {

template<typename T>
struct IsSharedPtrToReflectiveType {
private:
    using DecayType = std::decay_t<T>;

public:
    static constexpr bool value =
        IsSharedPtrV<DecayType> &&
        IsReflectiveTypeV<GetSharedPtrElementTypeT<DecayType>>;
};

template<typename T>
struct IsSharedPtrToCustomBoxedType {
public:
    using DecayType = std::decay_t<T>;

public:
    static constexpr bool value =
        IsSharedPtrV<DecayType> &&
        HasCustomBoxingTraitsV<std::decay_t<GetSharedPtrElementTypeT<DecayType>>>;
};

} //internal


/**
A template to convert the specified type to its corresponding boxed type.

@tparam T
    The type to convert.

@details
    If the specified type has a corresponding boxed type, the boxed type is defined as a type alias
    named `type` within the template; otherwise, nothing is defined in the template, and a compile 
    error will be emitted if the template is initiated, indicating that the specified type doesn't
    support boxing and unboxing.

    This template performs the conversion according to the following rules:
    - if `T` is a reflective type(`IsReflectiveTypeV<T>` is true), the result is the decayed type 
      of `T`;
    - if there is a specialization of CustomBoxingTraits for the decayed type of `T`
      (`HasCustomBoxingTraitsV<std::decay_t<T>>` is true), the result is the `BoxedType` defined in
      CustomBoxingTraits.

@see zaf::HasBoxedType
*/
template<typename T, typename = void>
struct ToBoxedType { };

/** @cond */
template<typename T>
struct ToBoxedType<T, std::enable_if_t<IsReflectiveTypeV<T>>> {
    using type = std::decay_t<T>;
};

template<typename T>
struct ToBoxedType<T, std::enable_if_t<HasCustomBoxingTraitsV<std::decay_t<T>>>> {
    using type = typename CustomBoxingTraits<std::decay_t<T>>::BoxedType;
};
/** @endcond */

/**
A helper type template for ToBoxedType.

@relates ToBoxedType
*/
template<typename T>
using ToBoxedTypeT = typename ToBoxedType<T>::type;


/**
A template to check if there is a corresponding boxed type for the specified type.

@tparam T
    The type to check.

@see zaf::ToBoxedType
*/
template<typename T, typename = void>
struct HasBoxedType : std::false_type { };

/** @cond */
template<typename T>
struct HasBoxedType<T, std::void_t<ToBoxedTypeT<T>>> : std::true_type { };
/** @endcond */

/**
A helper variable template for HasBoxedType.

@relates HasBoxedType
*/
template<typename T>
constexpr bool HasBoxedTypeV = HasBoxedType<T>::value;


/**
A template to convert the specified type to a boxed instance type.

@tparam T
    The type to convert.

@details
    A boxed instance type is a std::shared_ptr to a boxed type.

    If the specified type can be converted to a boxed instance type, the result is defined as a 
    type alias named `type` within the template; otherwise, nothing is defined in the template, and
    a compile error will be emitted if the template is initiated, indicating that the specified 
    type doesn't support boxing and unboxing.

    This template performs the conversion according to the following rules:
    - if `T` is a reflective type(`IsReflectiveTypeV<T>` is true), the result is std::shared_ptr to
      the decayed type of `T`;
    - if there is a specialization of CustomBoxingTraits for the decayed type of `T`
      (`HasCustomBoxingTraitsV<std::decay_t<T>>` is true), the result is std::shared_ptr to the 
      `BoxedType` defined in CustomBoxingTraits;
    - if `T` is a std::shared_ptr to a reflective type, the result is std::shared_ptr to the 
      decayed type of `T::element_type`;
    - if `T` is a std::shared_ptr to a type for which a specialization of CustomBoxingTraits is 
      defined(`HasCustomBoxingTraitsV<std::decay_t<T::element_type>>` is true), the result is 
      std::shared_ptr to the `BoxedType` defined in CustomBoxingTraits.

@see zaf::HasBoxedInstanceType
*/
template<typename T, typename = void>
struct ToBoxedInstanceType { };

/** @cond */
template<typename T>
struct ToBoxedInstanceType<T, std::enable_if_t<IsReflectiveTypeV<T>>> {
    using type = std::shared_ptr<std::decay_t<T>>;
};

template<typename T>
struct ToBoxedInstanceType<T, std::enable_if_t<HasCustomBoxingTraitsV<std::decay_t<T>>>> {
    using type = std::shared_ptr<ToBoxedTypeT<T>>;
};

template<typename T>
struct ToBoxedInstanceType<T, std::enable_if_t<internal::IsSharedPtrToReflectiveType<T>::value>> {
    using type = std::shared_ptr<std::decay_t<typename T::element_type>>;
};

template<typename T>
struct ToBoxedInstanceType<T, std::enable_if_t<internal::IsSharedPtrToCustomBoxedType<T>::value>> {
    using type = std::shared_ptr<ToBoxedTypeT<std::decay_t<typename T::element_type>>>;
};
/** @endcond */

/**
A helper type template for ToBoxedInstanceType.

@relates ToBoxedInstanceType
*/
template<typename T>
using ToBoxedInstanceTypeT = typename ToBoxedInstanceType<T>::type;


/**
A template to check if the specified type can be converted a boxed instance type.

@tparam T
    The type to check.

@see zaf::ToBoxedInstanceType
*/
template<typename T, typename = void>
struct HasBoxedInstanceType : std::false_type { };

/** @cond */
template<typename T>
struct HasBoxedInstanceType<T, std::void_t<ToBoxedInstanceTypeT<T>>> : std::true_type { };
/** @endcond */

/**
A helper variable template for HasBoxedInstanceType.

@relates HasBoxedInstanceType
*/
template<typename T>
constexpr bool HasBoxedInstanceTypeV = HasBoxedInstanceType<T>::value;

}