#pragma once

#include <deque>
#include <functional>
#include <list>
#include <map>
#include <set>
#include <vector>

//Declaration
namespace zaf {

template<typename... T>
std::size_t CalculateHash(const T&... arguments);

namespace internal {

template<typename T>
std::size_t CalculateContainerHash(const T& container);

}
}


namespace std {

//Hash specialization for std::pair.
template<typename T1, typename T2>
struct hash<std::pair<T1, T2>> {
    std::size_t operator()(const std::pair<T1, T2>& pair) {
        return zaf::CalculateHash(pair.first, pair.second);
    }
};


//Hash specializations for non-associative containers.
#define ZAF_INTERNAL_DEFINE_NON_ASSOCIATIVE_CONTAINER_HASH(Container)                              \
template<typename T>                                                                               \
struct hash<Container<T>> {                                                                        \
    std::size_t operator()(const Container<T>& container) {                                        \
        return zaf::internal::CalculateContainerHash(container);                                   \
    }                                                                                              \
};
ZAF_INTERNAL_DEFINE_NON_ASSOCIATIVE_CONTAINER_HASH(std::deque)
ZAF_INTERNAL_DEFINE_NON_ASSOCIATIVE_CONTAINER_HASH(std::list)
ZAF_INTERNAL_DEFINE_NON_ASSOCIATIVE_CONTAINER_HASH(std::multiset)
ZAF_INTERNAL_DEFINE_NON_ASSOCIATIVE_CONTAINER_HASH(std::set)
ZAF_INTERNAL_DEFINE_NON_ASSOCIATIVE_CONTAINER_HASH(std::vector)


//Hash specializations for associative containers.
#define ZAF_INTERNAL_DEFINE_ASSOCIATIVE_CONTAINER_HASH(Container)                                  \
template<typename K, typename V>                                                                   \
struct hash<Container<K, V>> {                                                                     \
    std::size_t operator()(const Container<K, V>& container) {                                     \
        return zaf::internal::CalculateContainerHash(container);                                   \
    }                                                                                              \
};
ZAF_INTERNAL_DEFINE_ASSOCIATIVE_CONTAINER_HASH(std::map)
ZAF_INTERNAL_DEFINE_ASSOCIATIVE_CONTAINER_HASH(std::multimap)

}


//Implementation
namespace zaf {
namespace internal {

constexpr std::size_t InitialHashValue = 17;

template<typename T>
std::size_t AccumulateHash(std::size_t hash, const T& value) {
    return hash * 31 + std::hash<T>()(value);
}


constexpr std::size_t InnerCalculateHash(std::size_t previous_hash) {
    return previous_hash;
}


template<typename T, typename... Ts>
std::size_t InnerCalculateHash(std::size_t previous_hash, const T& first, const Ts&... others) {

    std::size_t result = AccumulateHash(previous_hash, first);
    return InnerCalculateHash(result, others...);
}


template<typename T>
std::size_t CalculateContainerHash(const T& container) {

    std::size_t result = InitialHashValue;
    for (const auto& each_value : container) {
        result = AccumulateHash(result, each_value);
    }
    return result;
}

}

template<typename... T>
std::size_t CalculateHash(const T&... arguments) {
    return internal::InnerCalculateHash(internal::InitialHashValue, arguments...);
}

}