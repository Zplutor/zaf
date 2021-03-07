#include <gtest/gtest.h>
#include <zaf/resource/internal/resource_location_parsing.h>
#include <zaf/resource/internal/uri_parsing.h>

using namespace zaf;

TEST(ParseURITest, ParseURI) {

    auto result = internal::ParseURI(L"");
    ASSERT_EQ(result.type, internal::URIType::Unknown);

    result = internal::ParseURI(L"file://");
    ASSERT_EQ(result.type, internal::URIType::Unknown);

    result = internal::ParseURI(L"file://C:\\Windows\\test.txt");
    ASSERT_EQ(result.type, internal::URIType::Unknown);

    result = internal::ParseURI(L"file:///");
    ASSERT_EQ(result.type, internal::URIType::Unknown);

    result = internal::ParseURI(L"file:///test.txt");
    ASSERT_EQ(result.type, internal::URIType::Unknown);

    result = internal::ParseURI(L"file:///C:\\Windows\\test.txt");
    ASSERT_EQ(result.type, internal::URIType::File);
    ASSERT_EQ(result.value, L"C:\\Windows\\test.txt");

    result = internal::ParseURI(L"res://");
    ASSERT_EQ(result.type, internal::URIType::Unknown);

    result = internal::ParseURI(L"res:///");
    ASSERT_EQ(result.type, internal::URIType::Resource);
    ASSERT_EQ(result.value, L"/");

    result = internal::ParseURI(L"res:///main_window.xaml");
    ASSERT_EQ(result.type, internal::URIType::Resource);
    ASSERT_EQ(result.value, L"/main_window.xaml");

    result = internal::ParseURI(L"res://ADll/main_window.xaml");
    ASSERT_EQ(result.type, internal::URIType::Resource);
    ASSERT_EQ(result.value, L"ADll/main_window.xaml");

    result = internal::ParseURI(L"C:\\Windows\\test.txt");
    ASSERT_EQ(result.type, internal::URIType::File);
    ASSERT_EQ(result.value, L"C:\\Windows\\test.txt");

    result = internal::ParseURI(L"Windows\\test.txt");
    ASSERT_EQ(result.type, internal::URIType::Relative);
    ASSERT_EQ(result.value, L"Windows\\test.txt");

    result = internal::ParseURI(L"test.txt");
    ASSERT_EQ(result.type, internal::URIType::Relative);
    ASSERT_EQ(result.value, L"test.txt");
}


TEST(ParseURITest, ParseResourceLocation) {

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