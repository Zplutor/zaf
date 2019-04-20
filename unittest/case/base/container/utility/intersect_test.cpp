#include <list>
#include <set>
#include <gtest/gtest.h>
#include <zaf/base/container/utility/intersect.h>

TEST(Intersect, DefaultOutputType) {

    std::vector<int> c1{ 0, 1, 2, 3, 4, 5 };
    std::vector<int> c2{ 1, 3, 5, 7, 9 };
    auto intersection = zaf::Intersect(c1, c2);
    std::vector<int> expected{ 1, 3, 5 };
    ASSERT_EQ(intersection, expected);
}


TEST(Intersect, CustomizedOutputType) {

    std::vector<std::string> c1{ "0", "1", "2", "3", "4", "5" };
    std::vector<std::string> c2{ "1", "3", "5", "7", "9" };
    auto intersection = zaf::Intersect<std::set>(c1, c2);
    ASSERT_EQ(intersection.size(), 3);
    ASSERT_NE(intersection.find("1"), intersection.end());
    ASSERT_NE(intersection.find("3"), intersection.end());
    ASSERT_NE(intersection.find("5"), intersection.end());
}


TEST(Intersect, DifferentInputType) {

    std::vector<int> c1{ 0, 1, 2, 3, 4, 5 };
    std::set<int> c2{ 1, 3, 5, 7, 9 };
    auto intersection = zaf::Intersect<std::list>(c1, c2);
    std::list<int> expected{ 1, 3, 5 };
    ASSERT_EQ(intersection, expected);
}


TEST(Intersect, Predicate) {

    std::vector<int> c1{ 5, 4, 3, 2, 1, 0 };
    std::vector<int> c2{ 9, 7, 5, 3, 1, };
    auto intersection = zaf::Intersect(c1, c2, [](int v1, int v2) {
        return v1 > v2;
    });
    std::vector<int> expected{ 5, 3, 1 };
    ASSERT_EQ(intersection, expected);
}