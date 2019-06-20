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

    auto xaml = "<Point><Point.X>55</Point.X><Point.Y>66</Point.Y></Point>";
    auto node = zaf::XamlReader::CreateFromString(xaml)->Read();

    zaf::Point point;
    zaf::PointParser parser;
    parser.ParseFromNode(node, point);

    ASSERT_EQ(point, zaf::Point(55, 66));
}