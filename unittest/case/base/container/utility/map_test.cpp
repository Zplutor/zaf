#include <map>
#include <string>
#include <unordered_map>
#include <gtest/gtest.h>
#include <zaf/base/container/utility/map.h>

namespace internal {

template<template<typename K, typename V, typename ...> class C>
bool TestGetAllKeys() {

    C<int, std::string> map{ { 1, "1" }, { 2, "2" }, { 3, "3" } };

    auto keys = zaf::GetAllKeys(map);
    if (keys.size() != 3) {
        return false;
    }

    return keys[0] == 1 && keys[1] == 2 && keys[2] == 3;
}


template<template<typename K, typename V, typename ...> class C>
bool TestGetAllValues() {

    C<int, std::string> map{ { 1, "1" }, { 2, "2" }, { 3, "3" } };

    auto keys = zaf::GetAllValues(map);
    if (keys.size() != 3) {
        return false;
    }

    return keys[0] == "1" && keys[1] == "2" && keys[2] == "3";
}

}

TEST(Map, GetAllKeys) {

    ASSERT_TRUE(internal::TestGetAllKeys<std::map>());
    ASSERT_TRUE(internal::TestGetAllKeys<std::multimap>());
    ASSERT_TRUE(internal::TestGetAllKeys<std::unordered_map>());
    ASSERT_TRUE(internal::TestGetAllKeys<std::unordered_multimap>());
}


TEST(Map, GetAllValues) {

    ASSERT_TRUE(internal::TestGetAllKeys<std::map>());
    ASSERT_TRUE(internal::TestGetAllKeys<std::multimap>());
    ASSERT_TRUE(internal::TestGetAllKeys<std::unordered_map>());
    ASSERT_TRUE(internal::TestGetAllKeys<std::unordered_multimap>());
}