#pragma once

#include <type_traits>

namespace zaf::internal {

template<typename T>
struct GetBuiltInBoxType {
    using Type = void;
};


template<typename T>
struct HasBuiltInBoxType {
    static constexpr bool Value = !std::is_void_v<typename GetBuiltInBoxType<T>::Type>;
};


#define __ZAF_INTERNAL_DEFINE_BUILT_IN_BOXED_TYPE(OriginalType, BoxType)                           \
template<>                                                                                         \
struct zaf::internal::GetBuiltInBoxType<OriginalType> {                                            \
    using Type = BoxType;                                                                          \
};

}