#pragma once

#include <string>
#include <vector>

namespace zaf {

template<typename S>
std::vector<S> Split(const S& string, typename S::value_type delimiter) {

    std::vector<S> result;

    std::size_t current_index{};
    while (current_index <= string.length()) {

        auto delimiter_index = string.find(delimiter, current_index);
        if (delimiter_index == string.npos) {
            result.push_back(string.substr(current_index));
            break;
        }

        result.push_back(string.substr(current_index, delimiter_index - current_index));
        current_index = delimiter_index + 1;
    }

    return result;
}


template<typename C>
std::vector<std::basic_string<C>> Split(const C* string, C delimiter) {
    return Split(std::basic_string<C>(string), delimiter);
}

}