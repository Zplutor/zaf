#include <gtest/gtest.h>
#include <zaf/parsing/parsers/ellipse_parser.h>
#include <zaf/graphic/ellipse.h>
#include <zaf/parsing/xaml_node.h>
#include <zaf/parsing/xaml_reader.h>

TEST(EllipseParser, ParseFromNode) {

    auto xaml = R"(
        <Ellipse position="40,41" xRadius="9" yRadius="10" />
    )";
    auto node = zaf::XamlReader::CreateFromString(xaml)->Read();

    zaf::Ellipse ellipse;
    zaf::EllipseParser parser;
    parser.ParseFromNode(node, ellipse);
    ASSERT_EQ(ellipse, zaf::Ellipse(40, 41, 9, 10));

    xaml = R"(
        <Ellipse>
            <Ellipse.Position x="42" y="43" />
            <Ellipse.XRadius>11</Ellipse.XRadius>
            <Ellipse.YRadius>12</Ellipse.YRadius>
        </Ellipse>
    )";
    node = zaf::XamlReader::CreateFromString(xaml)->Read();

    parser.ParseFromNode(node, ellipse);
    ASSERT_EQ(ellipse, zaf::Ellipse(42, 43, 11, 12));
}