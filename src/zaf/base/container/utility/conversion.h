#pragma once

#include <list>
#include <set>
#include <vector>

namespace zaf {

template<typename C>
std::vector<typename C::value_type> MakeVector(const C& container) {
    return std::vector<typename C::value_type>{ container.begin(), container.end() };
}


template<typename C>
std::set<typename C::value_type> MakeSet(const C& container) {
    return std::set<typename C::value_type>{ container.begin(), container.end() };
}


template<typename C>
std::list<typename C::value_type> MakeList(const C& container) {
    return std::list<typename C::value_type>{ container.begin(), container.end() };
}

}