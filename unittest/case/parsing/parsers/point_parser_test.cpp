#include <gtest/gtest.h>
#include <zaf/graphic/point.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/parsing/xaml_reader.h>

TEST(PointParser, ParseFromAttribute) {

    zaf::Point point;
    point.GetType()->GetParser()->ParseFromAttribute(L"33,44", point);

    ASSERT_EQ(point, zaf::Point(33, 44));
}


TEST(PointParser, ParseFromNode) {

    auto xaml = R"(
        <Point X="53" Y="54" />
    )";
    auto node = zaf::XamlReader::FromString(xaml)->Read();

    zaf::Point point;
    auto parser = zaf::Point::Type->GetParser();
    parser->ParseFromNode(*node, point);
    ASSERT_EQ(point, zaf::Point(53, 54));

    xaml = R"(
        <Point>
            <Point.X>55</Point.X>
            <Point.Y>66</Point.Y>
        </Point>
    )";
    node = zaf::XamlReader::FromString(xaml)->Read();
    parser->ParseFromNode(*node, point);
    ASSERT_EQ(point, zaf::Point(55, 66));
}