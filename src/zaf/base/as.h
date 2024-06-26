#pragma once

#include <memory>
#include <type_traits>
#include <zaf/base/error/invalid_type_error.h>
#include <zaf/internal/object/shared_ptr_utility.h>

namespace zaf {
namespace internal {

struct NonSharedPtrCast {

    template<typename T, typename K>
    static decltype(auto) As(K&& value) {

        using PointerType = 
            std::conditional_t<std::is_const_v<std::remove_reference_t<K>>, const T*, T*>;

        auto result = dynamic_cast<PointerType>(&value);
        if (!result) {
            throw InvalidTypeError{ ZAF_SOURCE_LOCATION() };
        }
        return *result;
    }
};


struct SharedPtrCast {

    template<typename T, typename K>
    static std::shared_ptr<T> As(const K& value) {
        return std::dynamic_pointer_cast<T>(value);
    }
};


template<typename T>
struct CastSelector {
    using Type = std::conditional_t<
        IsSharedPtrV<std::decay_t<T>>, 
        SharedPtrCast,
        NonSharedPtrCast
    >;
};

}

template<typename T, typename K>
const T* As(const K* value) {
    return dynamic_cast<const T*>(value);
}

template<typename T, typename K>
T* As(K* value) {
    return dynamic_cast<T*>(value);
}


template<typename T, typename K>
decltype(auto) As(K&& value) {

    using CastType = typename internal::CastSelector<K>::Type;
    return CastType::template As<T, K>(std::forward<K>(value));
}

}