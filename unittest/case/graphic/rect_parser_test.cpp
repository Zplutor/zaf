#include <gtest/gtest.h>
#include <zaf/graphic/rect.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/parsing/xaml_reader.h>

TEST(RectParserTest, ParseFromNode) {

    auto xaml = "<Rect Position=\"10,11\" Size=\"12,13\" />";
    auto node = zaf::XamlReader::FromString(xaml)->Read();

    zaf::Rect rect;
    auto parser = zaf::Rect::Type->GetParser();
    parser->ParseFromNode(*node, rect);
    ASSERT_EQ(rect, zaf::Rect(10, 11, 12, 13));

    xaml = 
        "<Rect>"
        "   <Rect.Position X=\"14\" Y=\"15\"></Rect.Position>"
        "   <Rect.Size>"
        "       <Size.Width>16</Size.Width>"
        "       <Size.Height>17</Size.Height>"
        "   </Rect.Size>"
        "</Rect>";
    node = zaf::XamlReader::FromString(xaml)->Read();

    parser->ParseFromNode(*node, rect);
    ASSERT_EQ(rect, zaf::Rect(14, 15, 16, 17));
}