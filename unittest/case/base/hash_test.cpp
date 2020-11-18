#include <gtest/gtest.h>
#include <zaf/base/container/utility/conversion.h>
#include <zaf/base/hash.h>

TEST(HashTest, CalculateHash) {

    int i = 11;
    float f = 89.2f;
    std::string s = "nothing";

    std::vector<std::size_t> hash_values{
        std::hash<int>()(i),
        std::hash<float>()(f),
        std::hash<std::string>()(s),
        zaf::CalculateHash(i, f, s),
        zaf::CalculateHash(i, f),
        zaf::CalculateHash(i, s),
        zaf::CalculateHash(f, s),
    };

    auto hash_set = zaf::MakeSet(hash_values);
    ASSERT_EQ(hash_set.size(), hash_values.size());
}


TEST(HashTest, StandardTypes) {

    std::pair<int, std::string> pair{ 44, "444" };
    ASSERT_EQ(zaf::CalculateHash(pair), zaf::CalculateHash(pair));

    std::deque<int> deque{ 1, 2, 3 };
    ASSERT_EQ(zaf::CalculateHash(deque), zaf::CalculateHash(deque));

    std::list<int> list{ 1, 2, 3 };
    ASSERT_EQ(zaf::CalculateHash(list), zaf::CalculateHash(list));

    std::vector<int> vector{ 1, 2, 3 };
    ASSERT_EQ(zaf::CalculateHash(vector), zaf::CalculateHash(vector));

    std::set<int> set{ 1, 2, 3 };
    ASSERT_EQ(zaf::CalculateHash(set), zaf::CalculateHash(set));

    std::multiset<int> multiset{ 1, 2, 3 };
    ASSERT_EQ(zaf::CalculateHash(multiset), zaf::CalculateHash(multiset));

    std::map<int, std::string> map{ { 0, "0" }, { 1, "1" }, { 2, "2"} };
    ASSERT_EQ(zaf::CalculateHash(map), zaf::CalculateHash(map));

    std::multimap<int, std::string> multimap{ { 0, "0" }, { 1, "1" }, { 2, "2"} };
    ASSERT_EQ(zaf::CalculateHash(multimap), zaf::CalculateHash(multimap));
}