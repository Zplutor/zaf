#include <gtest/gtest.h>
#include <zaf/base/error/invalid_type_error.h>
#include <zaf/dynamic/parsing/parse_error.h>
#include <zaf/dynamic/boxing/boxing.h>
#include <zaf/dynamic/boxing/numeric.h>
#include <zaf/dynamic/object_type.h>
#include <zaf/dynamic/parsing/object_parser.h>
#include <zaf/dynamic/parsing/xaml_reader.h>

TEST(NumericBoxedTypeTest, IsEqual) {

    int i1 = 12;
    int i2 = 12;
    int i3 = 13;

    auto boxed1 = zaf::Box(i1);
    auto boxed2 = zaf::Box(i2);
    auto boxed3 = zaf::Box(i3);

    ASSERT_TRUE(boxed1->IsEqual(*boxed2));
    ASSERT_FALSE(boxed1->IsEqual(*boxed3));
}


TEST(NumericBoxedTypeTest, Hash) {

    int i1 = 14;
    int i2 = 14;
    int i3 = 15;

    auto boxed1 = zaf::Box(i1);
    auto boxed2 = zaf::Box(i2);
    auto boxed3 = zaf::Box(i3);

    ASSERT_EQ(boxed1->Hash(), boxed2->Hash());
    ASSERT_NE(boxed1->Hash(), boxed3->Hash());
    ASSERT_NE(boxed2->Hash(), boxed3->Hash());
}


TEST(NumericBoxedTypeTest, Parse) {

    zaf::Int32 i32;

    auto parser = zaf::Int32::StaticType()->Parser();
    parser->ParseFromAttribute(L"11", i32);
    ASSERT_EQ(i32.Value(), 11);

    ASSERT_THROW(parser->ParseFromAttribute(L"abc", i32), zaf::ParseError);

    auto node = zaf::XamlReader::FromString("<Int32>31</Int32>")->Read();
    parser->ParseFromNode(*node, i32);
    ASSERT_EQ(i32.Value(), 31);

    node = zaf::XamlReader::FromString("<Int32></Int32>")->Read();
    ASSERT_THROW(parser->ParseFromNode(*node, i32), zaf::ParseError);

    node = zaf::XamlReader::FromString("<Int32>abc</Int32>")->Read();
    ASSERT_THROW(parser->ParseFromNode(*node, i32), zaf::ParseError);
}


TEST(NumericBoxedTypeTest, ParseToInvalidObject) {

    auto parser = zaf::Int32::StaticType()->Parser();
    zaf::dynamic::Object object;

    ASSERT_THROW(parser->ParseFromAttribute(L"33", object), zaf::InvalidTypeError);

    auto node = zaf::XamlReader::FromString("<Int32>444</Int32>")->Read();
    ASSERT_THROW(parser->ParseFromNode(*node, object), zaf::InvalidTypeError);
}