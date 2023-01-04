#include <gtest/gtest.h>
#include <zaf/base/string/encoding_conversion.h>

TEST(EncodingConversionTest, ToUTF8String) {

    auto result = zaf::ToUTF8String(L"abcdef");
    ASSERT_EQ(result.size(), 6);
    ASSERT_EQ(result, "abcdef");
}


TEST(EncodingConversionTest, FromUTF8String) {

    auto result = zaf::FromUTF8String("1234567");
    ASSERT_EQ(result.size(), 7);
    ASSERT_EQ(result, L"1234567");
}