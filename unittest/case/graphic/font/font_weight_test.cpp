#include <gtest/gtest.h>
#include <zaf/base/error/invalid_type_error.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/creation.h>
#include <zaf/graphic/font/font_weight.h>
#include <zaf/object/parsing/helpers.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_reader.h>

TEST(FontWeightTest, Hash) {

    zaf::FontWeight weight1 = 111;
    zaf::FontWeight weight2 = 111;
    ASSERT_EQ(weight1.Hash(), weight2.Hash());
}


TEST(FontWeightTest, IsEqual) {

    zaf::FontWeight weight1 = zaf::FontWeight::ExtraBlack;
    zaf::FontWeight weight2 = zaf::FontWeight::ExtraBlack;
    ASSERT_TRUE(weight1.IsEqual(weight2));

    zaf::FontWeight weight3 = zaf::FontWeight::Bold;
    ASSERT_FALSE(weight1.IsEqual(weight3));
}


TEST(FontWeightTest, ToString) {

    zaf::FontWeight weight = 173;
    ASSERT_EQ(weight.ToString(), L"173");
}


TEST(FontWeightTest, ParseInvalidAttributeValue) {

    auto parser = zaf::FontWeight::StaticType()->GetParser();

    zaf::FontWeight font_weight;
    ASSERT_THROW(parser->ParseFromAttribute(L"", font_weight), zaf::ParseError);
    ASSERT_THROW(parser->ParseFromAttribute(L"3223ddsf", font_weight), zaf::ParseError);
}


TEST(FontWeightTest, ParseInvalidPropertyNodeValue) {

    auto xaml = LR"(<FontWeight></FontWeight>)";
    ASSERT_THROW(zaf::CreateObjectFromXaml<zaf::FontWeight>(xaml), zaf::ParseError);

    xaml = LR"(<FontWeight><ChildNode/></FontWeight>)";
    ASSERT_THROW(zaf::CreateObjectFromXaml<zaf::FontWeight>(xaml), zaf::ParseError);
}


TEST(FontWeightTest, ParseToInvalidObject) {

    auto parser = zaf::FontWeight::StaticType()->GetParser();
    zaf::Object object;

    ASSERT_THROW(parser->ParseFromAttribute(L"100", object), zaf::InvalidTypeError);

    auto xaml_node = zaf::XamlReader::FromString(L"<FontWeight>11</FontWeight>")->Read();
    ASSERT_THROW(parser->ParseFromNode(*xaml_node, object), zaf::InvalidTypeError);
}