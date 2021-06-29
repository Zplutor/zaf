#include <gtest/gtest.h>
#include <zaf/object/parsing/xaml_reader.h>

TEST(XamlReader, ReadFromWideString) {

    auto xaml_reader = zaf::XamlReader::FromString(L"<Tag>≤‚ ‘</Tag>");
    ASSERT_NE(xaml_reader, nullptr);

    auto node = xaml_reader->Read();
    ASSERT_NE(xaml_reader, nullptr);
}