#include <list>
#include <set>
#include <gtest/gtest.h>
#include <zaf/base/container/utility/differ.h>

TEST(Differ, Normal) {

    std::vector<int> c1{ 0, 1, 2, 3, 4, 5 };
    std::vector<int> c2{ 1, 3, 5, 7, 9 };
    auto difference = zaf::Differ(c1, c2);
    std::vector<int> expected{ 0, 2, 4 };
    ASSERT_EQ(difference, expected);
}


TEST(Differ, Predicate) {

    std::vector<int> c1{ 5, 4, 3, 2, 1, 0 };
    std::vector<int> c2{ 9, 7, 5, 3, 1, };
    auto difference = zaf::Differ(c1, c2, [](int v1, int v2) {
        return v1 > v2;
    });
    std::vector<int> expected{ 4, 2, 0 };
    ASSERT_EQ(difference, expected);
}