#include <gtest/gtest.h>
#include <zaf/graphic/point.h>
#include <zaf/parsing/parsers/point_parser.h>
#include <zaf/parsing/xaml_node.h>
#include <zaf/parsing/xaml_reader.h>

TEST(PointParser, ParseFromAttribute) {

    zaf::Point point;

    zaf::PointParser parser;
    parser.ParseFromAttribute(L"33,44", point);

    ASSERT_EQ(point, zaf::Point(33, 44));
}


TEST(PointParser, ParseFromNode) {

    auto xaml = R"(
        <Point X="53" Y="54" />
    )";
    auto node = zaf::XamlReader::FromString(xaml)->Read();

    zaf::Point point;
    zaf::PointParser parser;
    parser.ParseFromNode(*node, point);
    ASSERT_EQ(point, zaf::Point(53, 54));

    xaml = R"(
        <Point>
            <Point.X>55</Point.X>
            <Point.Y>66</Point.Y>
        </Point>
    )";
    node = zaf::XamlReader::FromString(xaml)->Read();
    parser.ParseFromNode(*node, point);
    ASSERT_EQ(point, zaf::Point(55, 66));
}