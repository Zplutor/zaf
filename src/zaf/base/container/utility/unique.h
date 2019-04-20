#pragma once

#include <algorithm>
#include <functional>

namespace zaf {

template<typename C>
void Unique(C& container) {
	container.erase(std::unique(container.begin(), container.end()), container.end());
}

template<typename C, typename P>
void Unique(C& container, P equal_predicate) {
    container.erase(std::unique(container.begin(), container.end(), std::move(equal_predicate)), container.end());
}

}