#pragma once

#include <algorithm>
#include <iterator>
#include <vector>

namespace zaf {

template<template<typename, typename...> typename Out = std::vector, typename In1, typename In2>
Out<typename In1::value_type> Union(const In1& container1, const In2& container2) {
    Out<typename In1::value_type> result;
    std::set_union(
        container1.begin(), container1.end(),
        container2.begin(), container2.end(),
        std::inserter(result, result.end()));
    return result;
}


template<template<typename, typename...> typename Out = std::vector, typename In1, typename In2, typename Predicate>
Out<typename In1::value_type> Union(const In1& container1, const In2& container2, Predicate predicate) {
    Out<typename In1::value_type> result;
    std::set_union(
        container1.begin(), container1.end(),
        container2.begin(), container2.end(),
        std::inserter(result, result.end()),
        predicate);
    return result;
}

}