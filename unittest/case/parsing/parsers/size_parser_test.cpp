#include <gtest/gtest.h>
#include <zaf/graphic/size.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/parsing/xaml_reader.h>

TEST(SizeParser, ParseFromAttribute) {

    zaf::Size size;
    size.GetType()->GetParser()->ParseFromAttribute(L"100,101", size);

    ASSERT_EQ(size, zaf::Size(100, 101));
}


TEST(SizeParser, ParseFromNode) {

    auto xaml = R"(
        <Size Width="100" Height="101" />
    )";
    auto node = zaf::XamlReader::FromString(xaml)->Read();

    zaf::Size size;
    auto parser = zaf::Size::Type->GetParser();
    parser->ParseFromNode(*node, size);
    ASSERT_EQ(size, zaf::Size(100, 101));

    xaml = "<Size><Size.Width>102</Size.Width><Size.Height>103</Size.Height></Size>";
    node = zaf::XamlReader::FromString(xaml)->Read();

    parser->ParseFromNode(*node, size);
    ASSERT_EQ(size, zaf::Size(102, 103));
}