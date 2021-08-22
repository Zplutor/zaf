#include <vector>
#include <list>
#include <deque>
#include <gtest/gtest.h>
#include <zaf/base/container/utility/unique.h>

namespace {

template<template<typename E, typename ...> class C>
bool TestUnique() {

    C<int> container{ 0, 1, 1, 2, 2, 3, 3, 3 };

    std::size_t removed_count = zaf::Unique(container);
    if (removed_count != 4) {
        return false;
    }

    C<int> expected{ 0, 1, 2, 3 };
    return container == expected;
}


template<template<typename E, typename ...> class C>
bool TestUniqueWithPredicate() {

    C<int> container{ 0, 1, 99, 99, 99, 2, 99, 99, 3, 4 };

    std::size_t removed_count = zaf::Unique(container, [](int v1, int v2) {
        return v1 == v2;
    });
    if (removed_count != 3) {
        return false;
    }

    C<int> expected{ 0, 1, 99, 2, 99, 3, 4 };
    return container == expected;
}

}

TEST(Unique, Normal) {

    ASSERT_TRUE(TestUnique<std::vector>());
    ASSERT_TRUE(TestUnique<std::list>());
    ASSERT_TRUE(TestUnique<std::deque>());
}


TEST(Unique, Predicate) {

    ASSERT_TRUE(TestUniqueWithPredicate<std::vector>());
    ASSERT_TRUE(TestUniqueWithPredicate<std::list>());
    ASSERT_TRUE(TestUniqueWithPredicate<std::deque>());
}