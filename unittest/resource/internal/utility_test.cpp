#include <gtest/gtest.h>
#include <zaf/resource/internal/uri_parsing.h>

using namespace zaf;

TEST(ParseUriTest, ParseUri) {

    auto result = internal::ParseUri(L"");
    ASSERT_EQ(result.type, internal::UriType::Unknown);

    result = internal::ParseUri(L"file:///");
    ASSERT_EQ(result.type, internal::UriType::Unknown);

    result = internal::ParseUri(L"file:///test.txt");
    ASSERT_EQ(result.type, internal::UriType::Unknown);

    result = internal::ParseUri(L"file:///C:\\Windows\\test.txt");
    ASSERT_EQ(result.type, internal::UriType::File);

    result = internal::ParseUri(L"C:\\Windows\\test.txt");
    ASSERT_EQ(result.type, internal::UriType::File);

    result = internal::ParseUri(L"Windows\\test.txt");
    ASSERT_EQ(result.type, internal::UriType::Relative);

    result = internal::ParseUri(L"test.txt");
    ASSERT_EQ(result.type, internal::UriType::Relative);
}