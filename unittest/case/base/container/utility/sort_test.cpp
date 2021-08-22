#include <deque>
#include <vector>
#include <utility>
#include <gtest/gtest.h>
#include <zaf/base/container/utility/sort.h>

namespace {

using namespace std::rel_ops;

class Object {
public:
    Object(int v, const std::string& t) : value(v), tag(t) { }

    int value{};
    std::string tag{};
};

bool operator<(const Object& o1, const Object& o2) {
    return o1.value < o2.value;
}

bool operator==(const Object& o1, const Object& o2) {
    return (o1.value == o2.value) && (o1.tag == o2.tag);
}


template<template<typename E, typename ...> class C>
bool TestSort() {

    C<Object> container{ { 4, "" }, { 3, "" }, { 2, "" }, { 1, "" }, { 0, "" } };
    const C<Object> expected{ { 0, "" }, { 1, "" }, { 2, "" }, { 3, "" }, { 4, "" } };

    auto new_container = zaf::MakeSorted(container);
    if (new_container != expected) {
        return false;
    }

    zaf::Sort(container);
    return container == expected;
}


template<template<typename E, typename ...> class C>
bool TestSortWithPredicate() {

    C<Object> container{ { 0, "" }, { 1, "" }, { 2, "" }, { 3, "" }, { 4, "" } };
    const C<Object> expected{ { 4, "" }, { 3, "" }, { 2, "" }, { 1, "" }, { 0, "" } };

    auto predicate = [](const Object& v1, const Object& v2) {
        return v1 > v2;
    };

    auto new_container = zaf::MakeSorted(container, predicate);
    if (new_container != expected) {
        return false;
    }

    zaf::Sort(container, predicate);
    return container == expected;
}


template<template<typename E, typename ...> class C>
bool TestStableSort() {

    C<Object> container{ 
        { 0, "a" }, { 9, "b" }, { 9, "c" }, { 9, "d" }, { 2, "e" },
        { 9, "f" }, { 3, "g" }, { 9, "h" }, { 4, "i" }, { 1, "j" },
    };
    const C<Object> expected{
        { 0, "a" }, { 1, "j" }, { 2, "e" }, { 3, "g" }, { 4, "i" }, 
        { 9, "b" }, { 9, "c" }, { 9, "d" }, { 9, "f" }, { 9, "h" },
    };

    auto new_container = zaf::MakeStableSorted(container);
    if (new_container != expected) {
        return false;
    }

    zaf::StableSort(container);
    return container == expected;
}


template<template<typename E, typename ...> class C>
bool TestStableSortWithPredicate() {

    C<Object> container{
        { 0, "a" }, { 9, "b" }, { 9, "c" }, { 9, "d" }, { 2, "e" },
        { 9, "f" }, { 3, "g" }, { 9, "h" }, { 4, "i" }, { 1, "j" },
    };
    const C<Object> expected{
        { 9, "b" }, { 9, "c" }, { 9, "d" }, { 9, "f" }, { 9, "h" },
        { 4, "i" }, { 3, "g" }, { 2, "e" }, { 1, "j" }, { 0, "a" },
    };

    auto predicate = [](const Object& o1, const Object& o2) {
        return o1.value > o2.value;
    };

    auto new_container = zaf::MakeStableSorted(container, predicate);
    if (new_container != expected) {
        return false;
    }

    zaf::StableSort(container, predicate);
    return container == expected;
}

}

TEST(Sort, Normal) {

    ASSERT_TRUE(TestSort<std::vector>());
    ASSERT_TRUE(TestSort<std::deque>());
    ASSERT_TRUE(TestSort<std::list>());
}


TEST(Sort, Predicate) {

    ASSERT_TRUE(TestSortWithPredicate<std::vector>());
    ASSERT_TRUE(TestSortWithPredicate<std::deque>());
    ASSERT_TRUE(TestSortWithPredicate<std::list>());
}


TEST(Sort, Stable) {

    ASSERT_TRUE(TestStableSort<std::vector>());
    ASSERT_TRUE(TestStableSort<std::deque>());
    ASSERT_TRUE(TestStableSort<std::list>());
}


TEST(Sort, StableSortWithPredicate) {

    ASSERT_TRUE(TestStableSortWithPredicate<std::vector>());
    ASSERT_TRUE(TestStableSortWithPredicate<std::deque>());
    ASSERT_TRUE(TestStableSortWithPredicate<std::list>());
}