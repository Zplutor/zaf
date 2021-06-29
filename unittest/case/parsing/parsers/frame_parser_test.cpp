#include <gtest/gtest.h>
#include <zaf/graphic/frame.h>
#include <zaf/parsing/parsers/frame_parser.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/parsing/xaml_reader.h>

TEST(FrameParser, ParseFromAttribute) {

    zaf::Frame frame;
    zaf::FrameParser parser;
    parser.ParseFromAttribute(L"4,3,2,1", frame);
    ASSERT_EQ(frame, zaf::Frame(4, 3, 2, 1));

    parser.ParseFromAttribute(L"5", frame);
    ASSERT_EQ(frame, zaf::Frame(5, 5, 5, 5));
}


TEST(FrameParser, ParseFromNodeWithAttribute) {

    auto xaml = R"(
        <Frame Left="20" Top="21" Right="22" Bottom="23" />
    )";
    auto node = zaf::XamlReader::FromString(xaml)->Read();

    zaf::Frame frame;
    zaf::FrameParser parser;
    parser.ParseFromNode(*node, frame);
    ASSERT_EQ(frame, zaf::Frame(20, 21, 22, 23));

    xaml = R"(
        <Frame>
            <Frame.Left>24</Frame.Left>
            <Frame.Top>25</Frame.Top>
            <Frame.Right>26</Frame.Right>
            <Frame.Bottom>27</Frame.Bottom>
        </Frame>
    )";
    node = zaf::XamlReader::FromString(xaml)->Read();

    parser.ParseFromNode(*node, frame);
    ASSERT_EQ(frame, zaf::Frame(24, 25, 26, 27));
}


TEST(FrameParser, ParseFromNodeWithValue) {

    auto xaml = "<Frame>9,8,7,6</Frame>";
    auto node = zaf::XamlReader::FromString(xaml)->Read();
    zaf::Frame frame;
    zaf::FrameParser parser;
    parser.ParseFromNode(*node, frame);
    ASSERT_EQ(frame, zaf::Frame(9, 8, 7, 6));
}