#include <gtest/gtest.h>
#include <zaf/base/error/invalid_type_error.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/graphic/point.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/parsing/xaml_reader.h>

TEST(PointParserTest, ParseFromAttribute) {

    zaf::Point point;
    point.GetType()->GetParser()->ParseFromAttribute(L"33,44", point);

    ASSERT_EQ(point, zaf::Point(33, 44));
}


TEST(PointParserTest, ParseFromNode) {

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


TEST(PointParserTest, ParseToInvalidObject) {

    auto parser = zaf::Point::Type->GetParser();
    zaf::Object object;

    ASSERT_THROW(parser->ParseFromAttribute(L"120,34", object), zaf::InvalidTypeError);

    //auto xaml_node = zaf::XamlReader::FromString(L"<Point X=\"18\" />")->Read();
    //ASSERT_THROW(parser->ParseFromNode(*xaml_node, object), zaf::InvalidTypeError);
}


TEST(PointParserTest, ParseInvalidValue) {

    auto parser = zaf::Point::Type->GetParser();
    zaf::Point point;

    ASSERT_THROW(parser->ParseFromAttribute(L"dd,cc", point), zaf::ParseError);
}