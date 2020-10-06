#pragma once

#include <type_traits>

namespace zaf::internal {

template<typename T>
struct GetBuiltInBoxType {
    using Type = void;
};

#define ZAF_INTERNAL_DEFINE_BUILT_IN_BOX_TYPE(OriginalType, BoxType)  \
template<>                                                            \
struct internal::GetBuiltInBoxType<OriginalType> {                    \
    using Type = BoxType;                                             \
};

template<typename T>
struct HasBuiltInBoxType {
    static constexpr bool Value = !std::is_void_v<typename GetBuiltInBoxType<T>::Type>;
};

}