#pragma once

#include <algorithm>
#include "zaf/base/container/utility/classify.h"
#include "zaf/base/container/utility/sort.h"
#include "zaf/base/container/utility/unique.h"

namespace zaf {

template<typename C>
std::size_t Erase(C& container, const typename C::value_type& element) {

    static_assert(
        !IsAssociativeContainer<C>::Value, 
        "zaf::Erase() can not be used with associative containers.");

    std::size_t old_size = container.size();
    container.erase(std::remove(container.begin(), container.end(), element), container.end());
    return old_size - container.size();
}


template<typename C, typename P>
std::size_t EraseIf(C& container, P&& predicate) {

    static_assert(
        !IsAssociativeContainer<C>::Value, 
        "zaf::EraseIf() can not be used with associative containers.");

    std::size_t old_size = container.size();

    container.erase(
        std::remove_if(container.begin(), container.end(), std::forward<P>(predicate)), 
        container.end());

    return old_size - container.size();
}


template<typename C>
std::size_t EraseDuplicates(C& container) {
    Sort(container);
    return Unique(container);
}

}