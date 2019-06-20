#include <gtest/gtest.h>
#include <zaf/parsing/xaml_reader.h>

TEST(XamlReader, ReadFromWideString) {

    std::error_code error_code;
    auto xaml_reader = zaf::XamlReader::CreateFromString(L"<Tag>≤‚ ‘</Tag>", error_code);
    ASSERT_FALSE(error_code);
    ASSERT_NE(xaml_reader, nullptr);

    auto node = xaml_reader->Read(error_code);
    ASSERT_FALSE(error_code);
    ASSERT_NE(xaml_reader, nullptr);
}