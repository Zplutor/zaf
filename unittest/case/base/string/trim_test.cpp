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

    string = reinterpret_cast<const char*>(u8" �� ");
    zaf::Trim(string);
    ASSERT_EQ(string, reinterpret_cast<const char*>(u8"��"));
}


TEST(TrimTest, TrimWideString) {

    std::wstring string{ L" WW   " };
    zaf::Trim(string);
    ASSERT_EQ(string, L"WW");

    string = L" �� ";
    zaf::Trim(string);
    ASSERT_EQ(string, L"��");
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


TEST(TrimTest, TrimStart) {

    auto test = [](std::string_view input, std::string_view expected) {
        std::string string(input);
        zaf::TrimStart(string);
        return string == expected;
    };
    
    ASSERT_TRUE(test("", ""));
    ASSERT_TRUE(test(" ", ""));
    ASSERT_TRUE(test("  ", ""));
    ASSERT_TRUE(test("123", "123"));
    ASSERT_TRUE(test(" 123 ", "123 "));
    ASSERT_TRUE(test("  123 ", "123 "));
}


TEST(TrimTest, TrimEnd) {

    auto test = [](std::string_view input, std::string_view expected) {
        std::string string(input);
        zaf::TrimEnd(string);
        return string == expected;
    };

    ASSERT_TRUE(test("", ""));
    ASSERT_TRUE(test(" ", ""));
    ASSERT_TRUE(test("  ", ""));
    ASSERT_TRUE(test("123", "123"));
    ASSERT_TRUE(test(" 123 ", " 123"));
    ASSERT_TRUE(test(" 123  ", " 123"));
}