#pragma once

#include <algorithm>
#include "zaf/base/container/utility/classify.h"
#include "zaf/base/container/utility/sort.h"
#include "zaf/base/container/utility/unique.h"

namespace zaf {

template<typename C>
void Erase(C& container, const typename C::value_type& element) {

    static_assert(
        !IsAssociativeContainer<C>::Value, 
        "zaf::Erase() can not be used with associative containers.");

	container.erase(std::remove(container.begin(), container.end(), element), container.end());
}


template<typename C, typename P>
void EraseIf(C& container, P predicate) {

    static_assert(
        !IsAssociativeContainer<C>::Value, 
        "zaf::EraseIf() can not be used with associative containers.");

	container.erase(
        std::remove_if(container.begin(), container.end(), std::move(predicate)), 
        container.end());
}


template<typename C>
void EraseDuplicates(C& container) {
    Sort(container);
    Unique(container);
}

}