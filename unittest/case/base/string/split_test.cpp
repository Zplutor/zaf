#include <gtest/gtest.h>
#include <zaf/base/string/split.h>

namespace {

bool TestSplit(
    const std::string& string,
    char delimiter,
    const std::vector<std::string>& expected) {

    auto result = zaf::Split(string, delimiter);
    return result == expected;
}

}

TEST(Split, Split) {

    ASSERT_TRUE(TestSplit("", ' ', { "" }));
    ASSERT_TRUE(TestSplit("0123456789", ' ', { "0123456789" }));
    ASSERT_TRUE(TestSplit("0123 456 789", ' ', { "0123", "456", "789" }));
    ASSERT_TRUE(TestSplit(" 0123 456 789 ", ' ', { "", "0123", "456", "789", "" }));
    ASSERT_TRUE(TestSplit("  01   234  ", ' ', { "", "", "01", "", "", "234", "", "" }));
    ASSERT_TRUE(TestSplit(" ", ' ', { "", "" }));
    ASSERT_TRUE(TestSplit("    ", ' ', { "", "", "", "", "" }));
}


TEST(Split, WideString) {

    std::wstring string(L"1,2");
    auto result = zaf::Split(string, L',');
    std::vector<std::wstring> expected{ L"1", L"2" };
    ASSERT_EQ(result, expected);
}


TEST(Split, CharArray) {

    auto result0 = zaf::Split("123", ',');
    std::vector<std::string> expected0{ "123" };
    ASSERT_EQ(result0, expected0);

    auto result1 = zaf::Split(L"123", L',');
    std::vector<std::wstring> expected1{ L"123" };
    ASSERT_EQ(result1, expected1);
}