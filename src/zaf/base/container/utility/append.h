#pragma once

#include "zaf/base/container/utility/classify.h"

namespace zaf {
namespace internal {

template<typename C1, typename C2>
struct AssociativeContainerImplement {
    static void Append(C1& c1, const C2& c2) {
        c1.insert(c2.begin(), c2.end());
    }
};

template<typename C1, typename C2>
struct SequenceContainerImplement {
    static void Append(C1& c1, const C2& c2) {
        c1.insert(c1.end(), c2.begin(), c2.end());
    }
};

}

template<typename C1, typename C2>
void Append(C1& c1, const C2& c2) {
    return std::conditional_t<
        IsAssociativeContainer<C1>::Value,
        internal::AssociativeContainerImplement<C1, C2>,
        internal::SequenceContainerImplement<C1, C2>
    >::Append(c1, c2);
}

}