#pragma once

#include <algorithm>
#include <functional>

namespace zaf {

template<typename C>
void Unique(C& container) {
	std::sort(container.begin(), container.end());
	container.erase(std::unique(container.begin(), container.end()), container.end());
}

template<typename C, typename LP, typename EP>
void Unique(C& container, LP less_predicate, EP equal_predicate) {
    std::sort(container.begin(), container.end(), std::move(less_predicate));
    container.erase(std::unique(container.begin(), container.end(), std::move(equal_predicate)), container.end());
}


template<typename C>
C MakeUnique(C&& container) {
	C new_container{ std::forward<C>(container) };
	Unique(new_container);
	return new_container;
}

template<typename C, typename LP, typename EP>
C MakeUnique(C&& container, LP less_predicate, EP equal_predicate) {
    C new_container{ std::forward<C>(container) };
    Unique(new_container, std::move(less_predicate), std::move(equal_predicate));
    return new_container;
}

}