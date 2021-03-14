#pragma once

#include <string>
#include <vector>

namespace zaf {

template<typename S, typename P>
std::vector<S> SplitIf(const S& string, P predicate) {

    std::vector<S> result;

    auto begin_iterator = string.begin();
    auto end_iterator = string.end();

    auto current_iterator = string.begin();
    while (current_iterator <= end_iterator) {

        auto delimiter_iterator = std::find_if(current_iterator, end_iterator, predicate);
        if (delimiter_iterator == end_iterator) {
            result.push_back(string.substr(std::distance(begin_iterator, current_iterator)));
            break;
        }

        auto sub_string = string.substr(
            std::distance(begin_iterator, current_iterator), 
            std::distance(current_iterator, delimiter_iterator));

        result.push_back(sub_string);

        current_iterator = std::next(delimiter_iterator);
    }

    return result;
}


template<typename S>
std::vector<S> Split(const S& string, typename S::value_type delimiter) {
    return SplitIf(string, [delimiter](typename S::value_type ch) {
        return ch == delimiter;
    });
}


template<typename C>
std::vector<std::basic_string<C>> Split(const C* string, C delimiter) {
    return Split(std::basic_string<C>(string), delimiter);
}

}