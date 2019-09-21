#include <list>
#include <set>
#include <vector>
#include <gtest/gtest.h>
#include <zaf/base/container/utility/append.h>

TEST(Append, AssociativeContainer) {

    std::set<int> set{ 0 };
    std::vector<int> vector{ 1, 2, 3 };
    zaf::Append(set, vector);

    ASSERT_EQ(set.size(), 4);
    ASSERT_NE(set.find(0), set.end());
    ASSERT_NE(set.find(1), set.end());
    ASSERT_NE(set.find(2), set.end());
    ASSERT_NE(set.find(3), set.end());
}


TEST(Append, SequenceContainer) {

    std::vector<int> vector{ 0 };
    std::set<int> set{ 1, 2, 3 };
    zaf::Append(vector, set);

    ASSERT_EQ(vector.size(), 4);
    ASSERT_EQ(vector[0], 0);
    ASSERT_EQ(vector[1], 1);
    ASSERT_EQ(vector[2], 2);
    ASSERT_EQ(vector[3], 3);
}


TEST(Append, CompatibleElementType) {

    std::vector<long long> vector{ 0, 1 };
    std::list<int> list{ 2, 3 };
    zaf::Append(vector, list);

    ASSERT_EQ(vector.size(), 4);
    ASSERT_EQ(vector[0], 0);
    ASSERT_EQ(vector[1], 1);
    ASSERT_EQ(vector[2], 2);
    ASSERT_EQ(vector[3], 3);
}


TEST(Append, InitializerList) {

    std::vector<long long> vector;
    zaf::Append(vector, { 0, 1, 2, 3 });

    std::vector<long long> expected{ 0, 1, 2, 3 };
    ASSERT_EQ(vector, expected);
}