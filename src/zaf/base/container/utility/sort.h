#pragma once

#include <algorithm>

namespace zaf {

template<typename C>
void Sort(C& container) {
	std::sort(container.begin(), container.end());
}

template<typename C, typename P>
void Sort(C& container, P predicate) {
	std::sort(container.begin(), container.end(), std::move(predicate));
}


template<typename C>
C MakeSorted(C&& container) {
	C new_container{ std::forward<C>(container) };
	Sort(new_container);
	return new_container;
}

template<typename C, typename P>
C MakeSorted(C&& container, P predicate) {
	C new_container{ std::forward<C>(container) };
	Sort(container, std::move(predicate));
	return new_container;
}

}