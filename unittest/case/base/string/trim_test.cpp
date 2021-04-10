#include <gtest/gtest.h>
#include <zaf/base/string/trim.h>

TEST(TrimTest, TrimSpace) {

    std::string string{ "  \n a b  d  \t" };
    zaf::Trim(string);
    ASSERT_EQ(string, "a b  d");

    string = "";
    zaf::Trim(string);
    ASSERT_EQ(string, "");

    string = " \r \t\n ";
    zaf::Trim(string);
    ASSERT_EQ(string, "");

    string = u8" 我 ";
    zaf::Trim(string);
    ASSERT_EQ(string, u8"我");
}


TEST(TrimTest, TrimWideString) {

    std::wstring string{ L" WW   " };
    zaf::Trim(string);
    ASSERT_EQ(string, L"WW");

    string = L" 我 ";
    zaf::Trim(string);
    ASSERT_EQ(string, L"我");
}


TEST(TrimTest, ToTrimmed) {

    ASSERT_EQ(zaf::ToTrimmed(std::string("  12")), "12");
    ASSERT_EQ(zaf::ToTrimmed("  12"), "12");

    ASSERT_EQ(zaf::ToTrimmed(std::wstring(L"  23")), L"23");
    ASSERT_EQ(zaf::ToTrimmed(L"  23"), L"23");

    auto trimmed_string = zaf::ToTrimmed("#1#", [](char ch) {
        return ch == '#';
    });
    ASSERT_EQ(trimmed_string, "1");
    
    auto trimmed_wstring = zaf::ToTrimmed(L"#2#", [](wchar_t ch) {
        return ch == '#';
    });
    ASSERT_EQ(trimmed_wstring, L"2");
}