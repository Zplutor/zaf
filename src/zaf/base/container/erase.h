#pragma once

#include <algorithm>
#include <functional>

namespace zaf {

template<typename C>
void Erase(C& container, const typename C::value_type& element) {
	container.erase(std::remove(container.begin(), container.end(), element), container.end());
}

template<typename C>
void EraseIf(C& container, std::function<bool(const typename C::value_type&)> predicate) {
	container.erase(std::remove_if(container.begin(), container.end(), std::move(predicate)), container.end());
}

}