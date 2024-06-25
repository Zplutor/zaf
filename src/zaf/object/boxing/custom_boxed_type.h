#pragma once

#include <type_traits>

namespace zaf {

template<typename T>
struct GetCustomBoxedType { };

template<typename T>
using GetCustomBoxedTypeT = typename GetCustomBoxedType<T>::type;


template<typename T, typename = void>
struct HasCustomBoxedType : std::false_type { };

template<typename T>
struct HasCustomBoxedType<T, std::void_t<GetCustomBoxedTypeT<T>>> : std::true_type { };

template<typename T>
constexpr bool HasCustomBoxedTypeV = HasCustomBoxedType<T>::value;

}