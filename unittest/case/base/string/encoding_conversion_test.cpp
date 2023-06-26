#include <gtest/gtest.h>
#include <zaf/base/error/error.h>
#include <zaf/base/string/encoding_conversion.h>

TEST(EncodingConversionTest, ToUTF8String) {

    auto result = zaf::ToUTF8String(L"");
    ASSERT_EQ(result.size(), 0);

    result = zaf::ToUTF8String(L"abcdef");
    ASSERT_EQ(result.size(), 6);
    ASSERT_EQ(result, "abcdef");
}


TEST(EncodingConversionTest, ToUTF8StringInvalidChars) {

    std::wstring wide_string{ L'\xec12' };

    //Conversion will still succeeded with default flags.
    auto result = zaf::ToUTF8String(wide_string);
    ASSERT_FALSE(result.empty());

    //Conversion will fail with FailOnInvalidChars flag.
    ASSERT_THROW(
        zaf::ToUTF8String(wide_string, zaf::UTF8ConversionFlags::FailOnInvalidChars),
        zaf::Error);
}


TEST(EncodingConversionTest, FromUTF8String) {

    auto result = zaf::FromUTF8String("");
    ASSERT_EQ(result.size(), 0);

    result = zaf::FromUTF8String("1234567");
    ASSERT_EQ(result.size(), 7);
    ASSERT_EQ(result, L"1234567");

    result = zaf::FromUTF8String(u8"什么什么");
    ASSERT_EQ(result, L"什么什么");
}


TEST(EncodingConversionTest, FromUTF8StringInvalidChars) {

    std::u8string utf8_string{ u8"哈" };
    std::string invalid_string{
        reinterpret_cast<const char*>(utf8_string.data()),
        utf8_string.length() - 1
    };

    //Conversion will still succeeded with default flags.
    auto result = zaf::FromUTF8String(invalid_string);
    ASSERT_FALSE(result.empty());

    //Conversion will fail with FailOnInvalidChars flag.
    ASSERT_THROW(
        zaf::FromUTF8String(invalid_string, zaf::UTF8ConversionFlags::FailOnInvalidChars),
        zaf::Error);
}