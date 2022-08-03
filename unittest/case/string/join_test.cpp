#include <gtest/gtest.h>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <zaf/base/string/encoding_conversion.h>
#include <zaf/base/string/join.h>

namespace {

template<typename C>
void TestJoin(const C& container, const std::string& expected) {

    std::string string = zaf::JoinAsString(container);
    ASSERT_EQ(string, expected);

    std::wstring wstring = zaf::JoinAsWideString(container);
    ASSERT_EQ(wstring, zaf::FromUtf8String(expected));
}

}

TEST(JoinTest, Join) {

    std::vector<int> vector{ 1, 2, 3 };
    TestJoin(vector, "1 2 3");

    std::list<int> list{ 4, 5, 6 };
    TestJoin(list, "4 5 6");

    std::deque<int> deque{ 7, 8, 9 };
    TestJoin(deque, "7 8 9");

    std::set<int> set{ 10, 11, 12 };
    TestJoin(set, "10 11 12");

    std::multiset<int> multiset{ 13, 14, 15 };
    TestJoin(multiset, "13 14 15");
}


TEST(JoinTest, JoinDifferentSizeContainer) {

    std::vector<int> empty;
    auto string = zaf::JoinAsString(empty);
    ASSERT_EQ(string, "");

    std::list<int> single_element{ 102 };
    string = zaf::JoinAsString(single_element);
    ASSERT_EQ(string, "102");
}


TEST(JoinTest, JoinWithDelimiter) {

    std::vector<int> container{ 1, 2, 3, 4, 5 };

    std::string string = zaf::JoinAsString(container, ",");
    ASSERT_EQ(string, "1,2,3,4,5");

    std::wstring wstring = zaf::JoinAsWideString(container, L",");
    ASSERT_EQ(wstring, L"1,2,3,4,5");
}


TEST(JoinTest, JoinWithConverter) {

    std::map<int, std::string> container{
        { 1, "a" },
        { 2, "b" },
        { 3, "c" },
    };

    std::string string = zaf::JoinAsString(container, [](const auto& each_pair) {
        return std::to_string(each_pair.first) + each_pair.second;
    });
    ASSERT_EQ(string, "1a 2b 3c");

    std::wstring wstring = zaf::JoinAsWideString(container, [](const auto& each_pair) {
        return zaf::FromUtf8String(std::to_string(each_pair.first) + each_pair.second);
    });
    ASSERT_EQ(wstring, L"1a 2b 3c");
}


TEST(JoinTest, JoinWithDelimiterAndConverter) {

    std::multimap<std::string, int> container{
        { "aa", 100 },
        { "bb", 100 },
        { "cc", 100 },
        { "dd", 100 },
    };

    std::string string = zaf::JoinAsString(container, "/", [](const auto& each_pair) {
        return each_pair.first + std::to_string(each_pair.second);
    });
    ASSERT_EQ(string, "aa100/bb100/cc100/dd100");

    std::wstring wstring = zaf::JoinAsWideString(container, L"/", [](const auto& each_pair) {
        return zaf::FromUtf8String(each_pair.first + std::to_string(each_pair.second));
    });
    ASSERT_EQ(wstring, L"aa100/bb100/cc100/dd100");
}