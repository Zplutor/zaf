#pragma once

#include <type_traits>
#include <zaf/internal/object/shared_ptr_utility.h>
#include <zaf/object/boxing/boolean.h>
#include <zaf/object/boxing/custom_boxed_type.h>
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
        HasCustomBoxedTypeV<GetSharedPtrElementTypeT<DecayType>>;
};

} //internal


template<typename T, typename = void>
struct ToBoxedType { };

template<typename T>
struct ToBoxedType<T, std::enable_if_t<IsReflectiveTypeV<std::decay_t<T>>>> {
    using type = std::decay_t<T>;
};

template<typename T>
struct ToBoxedType<T, std::enable_if_t<HasCustomBoxedTypeV<std::decay_t<T>>>> {
    using type = GetCustomBoxedTypeT<std::decay_t<T>>;
};

template<typename T>
using ToBoxedTypeT = typename ToBoxedType<T>::type;


template<typename T, typename = void>
struct HasBoxedType : std::false_type { };

template<typename T>
struct HasBoxedType<T, std::void_t<ToBoxedTypeT<T>>> : std::true_type { };

template<typename T>
constexpr bool HasBoxedTypeV = HasBoxedType<T>::value;


template<typename T, typename = void>
struct ToBoxedInstanceType { };

template<typename T>
struct ToBoxedInstanceType<T, std::enable_if_t<IsReflectiveTypeV<std::decay_t<T>>>> {
    using type = std::shared_ptr<std::decay_t<T>>;
};

template<typename T>
struct ToBoxedInstanceType<T, std::enable_if_t<HasCustomBoxedTypeV<std::decay_t<T>>>> {
    using type = std::shared_ptr<ToBoxedTypeT<T>>;
};

template<typename T>
struct ToBoxedInstanceType<T, std::enable_if_t<internal::IsSharedPtrToReflectiveType<T>::value>> {
    using type = std::decay_t<T>;
};

template<typename T>
struct ToBoxedInstanceType<T, std::enable_if_t<internal::IsSharedPtrToCustomBoxedType<T>::value>> {
    using type = std::shared_ptr<ToBoxedTypeT<typename T::element_type>>;
};

template<typename T>
using ToBoxedInstanceTypeT = typename ToBoxedInstanceType<T>::type;


template<typename T, typename = void>
struct HasBoxedInstanceType : std::false_type { };

template<typename T>
struct HasBoxedInstanceType<T, std::void_t<ToBoxedInstanceTypeT<T>>> : std::true_type { };

template<typename T>
constexpr bool HasBoxedInstanceTypeV = HasBoxedInstanceType<T>::value;

}