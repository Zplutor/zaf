#pragma once

#include <algorithm>
#include <functional>

namespace zaf {

template<typename C>
void Sort(C& container) {
	std::sort(container.begin(), container.end());
}

template<typename C>
void Sort(C& container, std::function<bool(const typename C::value_type&, const typename C::value_type&)> predicate) {
	std::sort(container.begin(), container.end(), std::move(predicate));
}


template<typename C>
C ToSorted(C&& container) {
	C new_container{ std::forward<C>(container) };
	Sort(new_container);
	return new_container;
}

template<typename C>
void ToSorted(C&& container, std::function<bool(const typename C::value_type&, const typename C::value_type&)> predicate) {
	C new_container{ std::forward<C>(container) };
	Sort(container, std::move(predicate));
	return new_container;
}

}