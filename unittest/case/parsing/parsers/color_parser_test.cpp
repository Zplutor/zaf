#include <gtest/gtest.h>
#include <zaf/graphic/color.h>
#include <zaf/parsing/parsers/color_parser.h>
#include <zaf/parsing/xaml_node.h>
#include <zaf/parsing/xaml_reader.h>

TEST(ColorParser, ParseFromAttribute) {

    auto xaml = L"#112233";
    zaf::Color color;
    zaf::ColorParser parser;
    parser.ParseFromAttribute(xaml, color);
    ASSERT_EQ(color, zaf::Color::FromRGB(0x112233));

    xaml = L"#44556677";
    parser.ParseFromAttribute(xaml, color);
    ASSERT_EQ(color, zaf::Color::FromARGB(0x44556677));
}


TEST(ColorParser, ParseFromNode) {

    auto xaml = R"(<Color a="0.1" r="0.2" g="0.3" b="0.4"></Color>)";
    auto node = zaf::XamlReader::CreateFromString(xaml)->Read();

    zaf::Color color;
    zaf::ColorParser parser;
    parser.ParseFromNode(node, color);
    ASSERT_EQ(color, zaf::Color(0.2f, 0.3f, 0.4f, 0.1f));

    xaml = R"(
        <Color>
            <Color.A>0.5</Color.A>
            <Color.R>0.6</Color.R>
            <Color.G>0.7</Color.G>
            <Color.B>0.8</Color.B>
        </Color>
    )";
    node = zaf::XamlReader::CreateFromString(xaml)->Read();

    parser.ParseFromNode(node, color);
    ASSERT_EQ(color, zaf::Color(0.6f, 0.7f, 0.8f, 0.5f));
}