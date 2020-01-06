#include <gtest/gtest.h>
#include <zaf/parsing/parsers/size_parser.h>
#include <zaf/graphic/size.h>
#include <zaf/parsing/xaml_node.h>
#include <zaf/parsing/xaml_reader.h>

TEST(SizeParser, ParseFromAttribute) {

    zaf::Size size;

    zaf::SizeParser parser;
    parser.ParseFromAttribute(L"100,101", size);

    ASSERT_EQ(size, zaf::Size(100, 101));
}


TEST(SizeParser, ParseFromNode) {

    auto xaml = R"(
        <Size Width="100" Height="101" />
    )";
    auto node = zaf::XamlReader::CreateFromString(xaml)->Read();

    zaf::Size size;
    zaf::SizeParser parser;
    parser.ParseFromNode(*node, size);
    ASSERT_EQ(size, zaf::Size(100, 101));

    xaml = "<Size><Size.Width>102</Size.Width><Size.Height>103</Size.Height></Size>";
    node = zaf::XamlReader::CreateFromString(xaml)->Read();

    parser.ParseFromNode(*node, size);
    ASSERT_EQ(size, zaf::Size(102, 103));
}