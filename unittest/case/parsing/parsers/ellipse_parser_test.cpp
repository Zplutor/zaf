#include <gtest/gtest.h>
#include <zaf/graphic/ellipse.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/parsing/xaml_reader.h>

TEST(EllipseParser, ParseFromNode) {

    auto xaml = R"(
        <Ellipse Position="40,41" XRadius="9" YRadius="10" />
    )";
    auto node = zaf::XamlReader::FromString(xaml)->Read();

    zaf::Ellipse ellipse;
    auto parser = zaf::Ellipse::StaticType()->Parser();
    parser->ParseFromNode(*node, ellipse);
    ASSERT_EQ(ellipse, zaf::Ellipse(40, 41, 9, 10));

    xaml = R"(
        <Ellipse>
            <Ellipse.Position X="42" Y="43" />
            <Ellipse.XRadius>11</Ellipse.XRadius>
            <Ellipse.YRadius>12</Ellipse.YRadius>
        </Ellipse>
    )";
    node = zaf::XamlReader::FromString(xaml)->Read();

    parser->ParseFromNode(*node, ellipse);
    ASSERT_EQ(ellipse, zaf::Ellipse(42, 43, 11, 12));
}