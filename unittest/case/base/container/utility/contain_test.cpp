#include <deque>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <gtest/gtest.h>
#include <zaf/base/container/utility/contain.h>

namespace internal {

template<typename C, typename E>
bool TestContainer(const C& container, const E& existent_element, const E& inexistent_element) {
    bool is_contained = zaf::Contain(container, existent_element);
    if (!is_contained) {
        return false;
    }

    is_contained = zaf::Contain(container, inexistent_element);
    if (is_contained) {
        return false;
    }

    return true;
}


template<template<typename E, typename ...> class C>
bool TestRegularContainer() {
    C<int> container{ 0, 1, 2, 3 };
    return TestContainer(container, 0, 4);
}


template<template<typename K, typename V, typename ...> class C>
bool TestMapContainer() {
    C<int, int> container{ {0, 0}, {1, 1}, {2, 2}, {3, 3} };
    return TestContainer(container, 0, 4);
}


class TestElement {
public:
    TestElement(int v) : value(v) { }

    int value = 0;
};

int g_less_compare_count = 0;
int g_equal_compare_count = 0;

bool operator<(const TestElement& e1, const TestElement& e2) {
    g_less_compare_count++;
    return e1.value < e2.value;
}

bool operator==(const TestElement& e1, const TestElement& e2) {
    g_equal_compare_count++;
    return e1.value == e2.value;
}

}

TEST(Contain, RegularContainer) {

    ASSERT_TRUE(internal::TestRegularContainer<std::vector>());
    ASSERT_TRUE(internal::TestRegularContainer<std::list>());
    ASSERT_TRUE(internal::TestRegularContainer<std::deque>());
    ASSERT_TRUE(internal::TestRegularContainer<std::set>());
    ASSERT_TRUE(internal::TestRegularContainer<std::multiset>());
    ASSERT_TRUE(internal::TestRegularContainer<std::unordered_set>());
    ASSERT_TRUE(internal::TestRegularContainer<std::unordered_multiset>());
}


TEST(Contain, MapContainer) {
    
    ASSERT_TRUE(internal::TestMapContainer<std::map>());
    ASSERT_TRUE(internal::TestMapContainer<std::multimap>());
    ASSERT_TRUE(internal::TestMapContainer<std::unordered_map>());
    ASSERT_TRUE(internal::TestMapContainer<std::unordered_multimap>());
}


TEST(Contain, AssociativeContainerEffective) {

    std::vector<internal::TestElement> vector;
    std::set<internal::TestElement> set;

    int value = 0;
    for (value = 0; value < 100; ++value) {
        vector.push_back(value);
        set.insert(value);
    }

    internal::g_equal_compare_count = 0;
    zaf::Contain(vector, internal::TestElement{value});
    int vector_compare_count = internal::g_equal_compare_count;

    internal::g_less_compare_count = 0;
    zaf::Contain(set, internal::TestElement{value});
    int set_compare_count = internal::g_less_compare_count;

    ASSERT_NE(set_compare_count, 0); //Make sure operator< is called
    ASSERT_LT(set_compare_count, vector_compare_count);
}