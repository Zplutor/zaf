#pragma once

#include <algorithm>
#include <functional>

namespace zaf {

template<typename C>
void Unique(C& container) {
	std::sort(container.begin(), container.end());
	container.erase(std::unique(container.begin(), container.end()), container.end());
}


template<typename C>
C ToUnique(C&& container) {
	C new_container{ std::forward<C>(container) };
	Unique(new_container);
	return new_container;
}

}