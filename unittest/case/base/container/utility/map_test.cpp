#include <map>
#include <string>
#include <unordered_map>
#include <gtest/gtest.h>
#include <zaf/base/container/utility/map.h>

namespace internal {

template<template<typename K, typename V, typename ...> class C>
bool TestKeys() {

    C<int, std::string> map{ { 1, "1" }, { 2, "2" }, { 3, "3" } };
    std::vector<int> keys;
    for (auto key : zaf::Keys(map)) {
        keys.push_back(key);
    }
    std::vector<int> expected_keys{ 1, 2, 3};
    return keys == expected_keys;
}


template<template<typename K, typename V, typename ...> class C>
bool TestValues() {

    C<int, std::string> map{ { 1, "1" }, { 2, "2" }, { 3, "3" } };
    std::vector<std::string> values;
    for (const auto& value : zaf::Values(map)) {
        values.push_back(value);
    }
    std::vector<std::string> expected_values{ "1", "2", "3" };
    return values == expected_values;
}


template<template<typename K, typename V, typename ...> class C>
bool TestGetAllKeys() {

    C<int, std::string> map{ { 1, "1" }, { 2, "2" }, { 3, "3" } };

    auto keys = zaf::GetKeys(map);
    if (keys.size() != 3) {
        return false;
    }

    return keys[0] == 1 && keys[1] == 2 && keys[2] == 3;
}


template<template<typename K, typename V, typename ...> class C>
bool TestGetAllValues() {

    C<int, std::string> map{ { 1, "1" }, { 2, "2" }, { 3, "3" } };

    auto keys = zaf::GetValues(map);
    if (keys.size() != 3) {
        return false;
    }

    return keys[0] == "1" && keys[1] == "2" && keys[2] == "3";
}

}


TEST(Map, Keys) {

    ASSERT_TRUE(internal::TestKeys<std::map>());
    ASSERT_TRUE(internal::TestKeys<std::multimap>());
    ASSERT_TRUE(internal::TestKeys<std::unordered_map>());
    ASSERT_TRUE(internal::TestKeys<std::unordered_multimap>());
}


TEST(Map, Values) {

    ASSERT_TRUE(internal::TestValues<std::map>());
    ASSERT_TRUE(internal::TestValues<std::multimap>());
    ASSERT_TRUE(internal::TestValues<std::unordered_map>());
    ASSERT_TRUE(internal::TestValues<std::unordered_multimap>());
}


TEST(Map, GetKeys) {

    ASSERT_TRUE(internal::TestGetAllKeys<std::map>());
    ASSERT_TRUE(internal::TestGetAllKeys<std::multimap>());
    ASSERT_TRUE(internal::TestGetAllKeys<std::unordered_map>());
    ASSERT_TRUE(internal::TestGetAllKeys<std::unordered_multimap>());
}


TEST(Map, GetValues) {

    ASSERT_TRUE(internal::TestGetAllKeys<std::map>());
    ASSERT_TRUE(internal::TestGetAllKeys<std::multimap>());
    ASSERT_TRUE(internal::TestGetAllKeys<std::unordered_map>());
    ASSERT_TRUE(internal::TestGetAllKeys<std::unordered_multimap>());
}