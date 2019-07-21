#pragma once

#include <algorithm>
#include <zaf/base/container/utility/classify.h>

namespace zaf {
namespace internal {

template<typename C>
struct GeneralSorter {

    static void Sort(C& container) {
        std::sort(container.begin(), container.end());
    }

    template<typename P>
    static void Sort(C& container, P&& predicate) {
        std::sort(container.begin(), container.end(), std::forward<P>(predicate));
    }

    static void StableSort(C& container) {
        std::stable_sort(container.begin(), container.end());
    }

    template<typename P>
    static void StableSort(C& container, P&& predicate) {
        std::stable_sort(container.begin(), container.end(), std::forward<P>(predicate));
    }
};


template<typename C>
struct ListSorter {

    static void Sort(C& container) {
        container.sort();
    }

    template<typename P>
    static void Sort(C& container, P&& predicate) {
        container.sort(std::forward<P>(predicate));
    }

    static void StableSort(C& container) {
        Sort(container);
    }

    template<typename P>
    static void StableSort(C& container, P&& predicate) {
        Sort(container, std::forward<P>(predicate));
    }
};


template<typename C>
struct GetSorter {

    static_assert(
        !IsAssociativeContainer<C>::Value,
        "zaf::Sort() family functions can not be used with associative containers.");

    using Type = std::conditional_t<IsListContainer<C>::Value, ListSorter<C>, GeneralSorter<C>>;
};

template<typename C>
using Sorter = typename GetSorter<C>::Type;

}

template<typename C>
void Sort(C& container) {
    internal::Sorter<C>::Sort(container);
}

template<typename C, typename P>
void Sort(C& container, P&& predicate) {
    internal::Sorter<C>::Sort(container, std::forward<P>(predicate));
}


template<typename C>
void StableSort(C& container) {
    internal::Sorter<C>::StableSort(container);
}

template<typename C, typename P>
void StableSort(C& container, P&& predicate) {
    internal::Sorter<C>::StableSort(container, std::forward<P>(predicate));
}


template<typename C>
C MakeSorted(C&& container) {
	C new_container{ std::forward<C>(container) };
	Sort(new_container);
	return new_container;
}

template<typename C, typename P>
C MakeSorted(C&& container, P&& predicate) {
	C new_container{ std::forward<C>(container) };
	Sort(container, std::forward<P>(predicate));
	return new_container;
}


template<typename C>
C MakeStableSorted(C&& container) {
    C new_container{ std::forward<C>(container) };
    StableSort(new_container);
    return new_container;
}

template<typename C, typename P>
C MakeStableSorted(C&& container, P&& predicate) {
    C new_container{ std::forward<C>(container) };
    StableSort(container, std::forward<P>(predicate));
    return new_container;
}


}