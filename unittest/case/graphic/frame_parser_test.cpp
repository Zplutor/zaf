#include <gtest/gtest.h>
#include <zaf/base/error/invalid_type_error.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/graphic/frame.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/parsing/xaml_reader.h>

TEST(FrameParserTest, ParseFromAttribute) {

    zaf::Frame frame;
    auto parser = zaf::Frame::StaticType()->Parser();
    parser->ParseFromAttribute(L"4,3,2,1", frame);
    ASSERT_EQ(frame, zaf::Frame(4, 3, 2, 1));

    parser->ParseFromAttribute(L"5", frame);
    ASSERT_EQ(frame, zaf::Frame(5, 5, 5, 5));
}


TEST(FrameParserTest, ParseFromNodeWithAttribute) {

    auto xaml = R"(
        <Frame Left="20" Top="21" Right="22" Bottom="23" />
    )";
    auto node = zaf::XamlReader::FromString(xaml)->Read();

    zaf::Frame frame;
    auto parser = zaf::Frame::StaticType()->Parser();
    parser->ParseFromNode(*node, frame);
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

    parser->ParseFromNode(*node, frame);
    ASSERT_EQ(frame, zaf::Frame(24, 25, 26, 27));
}


TEST(FrameParserTest, ParseFromNodeWithValue) {

    auto xaml = "<Frame>9,8,7,6</Frame>";
    auto node = zaf::XamlReader::FromString(xaml)->Read();
    zaf::Frame frame;
    auto parser = zaf::Frame::StaticType()->Parser();
    parser->ParseFromNode(*node, frame);
    ASSERT_EQ(frame, zaf::Frame(9, 8, 7, 6));
}


TEST(FrameParserTest, ParseToInvalidObject) {

    auto parser = zaf::Frame::StaticType()->Parser();
    zaf::Object object;

    ASSERT_THROW(parser->ParseFromAttribute(L"1,1,1,1", object), zaf::InvalidTypeError);

    auto node = zaf::XamlReader::FromString(L"<Frame>2,2,2,2</Frame>")->Read();
    ASSERT_THROW(parser->ParseFromNode(*node, object), zaf::InvalidTypeError);
}


TEST(FrameParserTest, ParseInvalidValue) {

    auto parser = zaf::Frame::StaticType()->Parser();
    zaf::Frame frame;

    ASSERT_THROW(parser->ParseFromAttribute(L"uiok", frame), zaf::ParseError);
}