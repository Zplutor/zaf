#include <gtest/gtest.h>
#include <zaf/base/string/case_conversion.h>

TEST(CaseConversion, Uppercase) {

    std::string string("ABCdef");
    zaf::Uppercase(string);
    ASSERT_EQ(string, "ABCDEF");

    std::wstring wstring(L"ABCdef");
    zaf::Uppercase(wstring);
    ASSERT_EQ(wstring, L"ABCDEF");
}


TEST(CaseConversion, Lowercase) {

    std::string string("ABCdef");
    zaf::Lowercase(string);
    ASSERT_EQ(string, "abcdef");

    std::wstring wstring(L"ABCdef");
    zaf::Lowercase(wstring);
    ASSERT_EQ(wstring, L"abcdef");
}


TEST(CaseConversion, ToUppercased) {

    auto string = zaf::ToUppercased("ABCdef");
    ASSERT_EQ(string, "ABCDEF");

    auto wstring = zaf::ToUppercased(L"ABCdef");
    ASSERT_EQ(wstring, L"ABCDEF");
}


TEST(CcaseConversion, ToLowercased) {

    auto string = zaf::ToLowercased("ABCdef");
    ASSERT_EQ(string, "abcdef");

    auto wstring = zaf::ToLowercased(L"ABCdef");
    ASSERT_EQ(wstring, L"abcdef");
}