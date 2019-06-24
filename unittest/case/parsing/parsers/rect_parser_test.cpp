#include <gtest/gtest.h>
#include <zaf/parsing/parsers/rect_parser.h>
#include <zaf/graphic/rect.h>
#include <zaf/parsing/xaml_node.h>
#include <zaf/parsing/xaml_reader.h>

TEST(RectParser, ParseFromNode) {

    auto xaml = "<Rect position=\"10,11\" size=\"12,13\" />";
    auto node = zaf::XamlReader::CreateFromString(xaml)->Read();

    zaf::Rect rect;
    zaf::RectParser parser;
    parser.ParseFromNode(node, rect);
    ASSERT_EQ(rect, zaf::Rect(10, 11, 12, 13));

    xaml = 
        "<Rect>"
        "   <Rect.Position x=\"14\" y=\"15\"></Rect.Position>"
        "   <Rect.Size>"
        "       <Size.Width>16</Size.Width>"
        "       <Size.Height>17</Size.Height>"
        "   </Rect.Size>"
        "</Rect>";
    node = zaf::XamlReader::CreateFromString(xaml)->Read();

    parser.ParseFromNode(node, rect);
    ASSERT_EQ(rect, zaf::Rect(14, 15, 16, 17));
}