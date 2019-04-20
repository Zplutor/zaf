#include <list>
#include <set>
#include <gtest/gtest.h>
#include <zaf/base/container/utility/union.h>

TEST(Union, Normal) {

    std::vector<int> c1{ 0, 1, 2, 3, 4, 5 };
    std::vector<int> c2{ 1, 3, 5, 7, 9 };
    auto union_result= zaf::Union(c1, c2);
    std::vector<int> expected{ 0, 1, 2, 3, 4, 5, 7, 9 };
    ASSERT_EQ(union_result, expected);
}


TEST(Union, Predicate) {

    std::vector<int> c1{ 5, 4, 3, 2, 1, 0 };
    std::vector<int> c2{ 9, 7, 5, 3, 1, };
    auto union_result = zaf::Union(c1, c2, [](int v1, int v2) {
        return v1 > v2;
    });
    std::vector<int> expected{ 9, 7, 5, 4, 3, 2, 1, 0 };
    ASSERT_EQ(union_result, expected);
}