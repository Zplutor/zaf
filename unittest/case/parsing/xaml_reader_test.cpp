#include <gtest/gtest.h>
#include <zaf/parsing/xaml_reader.h>

TEST(XamlReader, ReadFromWideString) {

    auto xaml_reader = zaf::XamlReader::CreateFromString(L"<Tag>≤‚ ‘</Tag>");
    ASSERT_NE(xaml_reader, nullptr);

    auto node = xaml_reader->Read();
    ASSERT_NE(xaml_reader, nullptr);
}