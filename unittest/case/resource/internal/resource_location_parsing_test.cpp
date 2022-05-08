#include <gtest/gtest.h>
#include <zaf/resource/internal/resource_location_parsing.h>

using namespace zaf;

TEST(ResourceLocationParsingTest, ParseResourceLocation) {

    internal::ResourceLocation location;

    location = internal::ParseResourceLocation(L"");
    ASSERT_EQ(location.dll, L"");
    ASSERT_EQ(location.name, L"");

    location = internal::ParseResourceLocation(L"/");
    ASSERT_EQ(location.dll, L"");
    ASSERT_EQ(location.name, L"");

    location = internal::ParseResourceLocation(L"/something");
    ASSERT_EQ(location.dll, L"");
    ASSERT_EQ(location.name, L"something");

    location = internal::ParseResourceLocation(L"somedll/something");
    ASSERT_EQ(location.dll, L"somedll");
    ASSERT_EQ(location.name, L"something");

    location = internal::ParseResourceLocation(L"somedll/something/something2");
    ASSERT_EQ(location.dll, L"somedll");
    ASSERT_EQ(location.name, L"something/something2");
}