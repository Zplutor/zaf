#pragma once

#include <algorithm>
#include <functional>

namespace zaf {

template<typename C>
std::size_t Unique(C& container) {

    std::size_t old_size = container.size();
	container.erase(std::unique(container.begin(), container.end()), container.end());
    return old_size - container.size();
}

template<typename C, typename P>
std::size_t Unique(C& container, P&& equal_predicate) {

    std::size_t old_size = container.size();

    container.erase(
        std::unique(container.begin(), container.end(), std::forward<P>(equal_predicate)), 
        container.end());

    return old_size - container.size();
}

}