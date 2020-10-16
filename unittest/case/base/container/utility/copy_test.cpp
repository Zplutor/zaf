#include <deque>
#include <list>
#include <vector>
#include <gtest/gtest.h>
#include <zaf/base/container/utility/copy.h>

namespace {

template<template<typename E, typename ...> class C>
bool TestCopyIfWithValueContainer() {

    C<int> container{ 0, 1, 2, 3, 4, 5 };

    C<int> new_container = zaf::CopyIf(container, [](int value) {
        return value % 2 == 0;
    });

    C<int> expected_container{ 0, 2, 4 };
    return expected_container == new_container;
}

template<template<typename K, typename V, typename ...> class C>
bool TestCopyIfWithKeyValueContainer() {

    C<int, std::string> container{
        { 0, "A" },
        { 1, "B" },
        { 2, "C" },
        { 3, "D" },
        { 4, "E" },
    };

    C<int, std::string> new_container = zaf::CopyIf(container, [](const auto& pair) {
        return pair.first % 2 == 1;
    });

    C<int, std::string> expected_container{
        { 1, "B" },
        { 3, "D" },
    };
    return new_container == expected_container;
}

}


TEST(CopyTest, CopyIf) {

    ASSERT_TRUE(TestCopyIfWithValueContainer<std::vector>());
    ASSERT_TRUE(TestCopyIfWithValueContainer<std::list>());
    ASSERT_TRUE(TestCopyIfWithValueContainer<std::deque>());
    ASSERT_TRUE(TestCopyIfWithValueContainer<std::set>());
    ASSERT_TRUE(TestCopyIfWithValueContainer<std::multiset>());
    ASSERT_TRUE(TestCopyIfWithKeyValueContainer<std::map>());
    ASSERT_TRUE(TestCopyIfWithKeyValueContainer<std::multimap>());
}


TEST(CopyTest, CopyIfWithConstContainer) {

    const std::vector<int> container{ 0, 1, 2 };

    auto new_container = zaf::CopyIf(container, [](int value) {
        return value == 1;
    });

    std::vector<int> expected_container{ 1 };
    ASSERT_EQ(new_container, expected_container);
}