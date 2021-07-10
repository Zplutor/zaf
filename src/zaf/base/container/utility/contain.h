#pragma once

#include <type_traits>
#include "zaf/base/container/utility/classify.h"

namespace zaf {
namespace internal {

template<typename C, typename E>
struct FastImplement {
    static bool Contain(const C& container, const E& element) {
        return container.find(element) != container.end();
    }
};

template<typename C, typename E>
struct NormalImplement {
    static bool Contain(const C& container, const E& element) {
        return std::find(container.begin(), container.end(), element) != container.end();
    }
};

}


template<typename C, typename E>
bool Contain(const C& container, const E& element) {
    return std::conditional_t<
        IsAssociativeContainer<C>::Value,
        internal::FastImplement<C, E>,
        internal::NormalImplement<C, E>
    >::Contain(container, element);
}

}