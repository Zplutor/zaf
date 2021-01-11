#include <gtest/gtest.h>
#include <zaf/parsing/parsers/rounded_rect_parser.h>
#include <zaf/graphic/rounded_rect.h>
#include <zaf/parsing/xaml_node.h>
#include <zaf/parsing/xaml_reader.h>

TEST(RoundedRectParser, ParseFromNode) {

    std::string xaml = R"(
        <RoundedRect XRadius="0.5" YRadius="0.6">
            <RoundedRect.Rect Position="2,2" Size="5,6" />
        </RoundedRect>
    )";

    auto node = zaf::XamlReader::FromString(xaml)->Read();

    zaf::RoundedRect rounded_rect;
    zaf::RoundedRectParser parser;
    parser.ParseFromNode(*node, rounded_rect);
    ASSERT_EQ(rounded_rect, zaf::RoundedRect(zaf::Rect(2, 2, 5, 6), 0.5f, 0.6f));

    xaml = R"(
        <RoundedRect>
            <RoundedRect.XRadius>0.7</RoundedRect.XRadius>
            <RoundedRect.YRadius>0.8</RoundedRect.YRadius>
            <RoundedRect.Rect Position="3,3" Size="4,4" />
        </RoundedRect>
    )";

    node = zaf::XamlReader::FromString(xaml)->Read();

    parser.ParseFromNode(*node, rounded_rect);
    ASSERT_EQ(rounded_rect, zaf::RoundedRect(zaf::Rect(3, 3, 4, 4), 0.7f, 0.8f));
}