#pragma once

#include <vector>

namespace zaf {

template<typename C>
std::vector<typename C::key_type> GetAllKeys(const C& map) {
    std::vector<typename C::key_type> keys;
    for (const auto& each_pair : map) {
        keys.push_back(each_pair.first);
    }
    return keys;
}


template<typename C>
std::vector<typename C::mapped_type> GetAllValues(const C& map) {
    std::vector<typename C::mapped_type> values;
    for (const auto& each_pair : map) {
        values.push_back(each_pair.second);
    }
    return values;
}

}