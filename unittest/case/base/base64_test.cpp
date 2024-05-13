#include <gtest/gtest.h>
#include <zaf/base/base64.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/error/invalid_data_error.h>

namespace {

struct TestItem {
    std::string raw;
    std::wstring encoded;
};

std::vector<TestItem> g_test_items{
    { "", L"" },
    { "a", L"YQ==" },
    { "ab", L"YWI=" },
    { "abc", L"YWJj" },
    { "abcd", L"YWJjZA==" },
    { "abcde", L"YWJjZGU=" },
    { "abcdef", L"YWJjZGVm" },
};

}


TEST(Base64Test, Encode) {

    for (const auto& each_item : g_test_items) {
        auto encoded = zaf::Base64Encode(each_item.raw.data(), each_item.raw.length());
        ASSERT_EQ(encoded, each_item.encoded);
    }
}


TEST(Base64Test, Decode) {

    for (const auto& each_item : g_test_items) {

        auto decoded = zaf::Base64Decode(each_item.encoded);

        std::string_view decoded_string{
            reinterpret_cast<const char*>(decoded.Data()),
            decoded.Size()
        };
        ASSERT_EQ(decoded_string, each_item.raw);
    }
}


TEST(Base64Test, EncodeError) {

    //Data to be encoded is not allow to be null.
    ASSERT_THROW(zaf::Base64Encode(nullptr, 0), zaf::PreconditionError);
}


TEST(Base64Test, DecodeError) {

    //The encoded string has invalid length.
    ASSERT_THROW(zaf::Base64Decode(L"a"), zaf::InvalidDataError);
    ASSERT_THROW(zaf::Base64Decode(L"ab"), zaf::InvalidDataError);
    ASSERT_THROW(zaf::Base64Decode(L"abc"), zaf::InvalidDataError);
    ASSERT_THROW(zaf::Base64Decode(L"abcde"), zaf::InvalidDataError);

    //The encoded string has invalid chars.
    ASSERT_THROW(zaf::Base64Decode(L"ac-D"), zaf::InvalidDataError);

    //Invalid char composition.
    ASSERT_THROW(zaf::Base64Decode(L"YQ-="), zaf::InvalidDataError);
}


TEST(Base64Test, EncodeByteArray) {

    zaf::ByteArray byte_array;
    auto encoded_string = zaf::Base64Encode(byte_array);
    ASSERT_TRUE(encoded_string.empty());

    byte_array.Append(std::byte('1'));
    byte_array.Append(std::byte('2'));
    byte_array.Append(std::byte('3'));
    encoded_string = zaf::Base64Encode(byte_array);
    ASSERT_EQ(encoded_string, L"MTIz");
}