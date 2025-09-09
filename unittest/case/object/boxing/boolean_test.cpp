#include <gtest/gtest.h>
#include <zaf/base/error/invalid_type_error.h>
#include <zaf/dynamic/parsing/parse_error.h>
#include <zaf/dynamic/boxing/boolean.h>
#include <zaf/dynamic/object_type.h>
#include <zaf/dynamic/parsing/helpers.h>
#include <zaf/dynamic/parsing/object_parser.h>
#include <zaf/dynamic/parsing/xaml_reader.h>

TEST(BooleanTest, ParseAttribute) {

    auto parser = zaf::Boolean::StaticType()->Parser();

    zaf::Boolean b;
    parser->ParseFromAttribute(L"true", b);
    ASSERT_EQ(b.Value(), true);

    parser->ParseFromAttribute(L"false", b);
    ASSERT_EQ(b.Value(), false);

    ASSERT_THROW(parser->ParseFromAttribute(L"0", b), zaf::ParseError);
    ASSERT_THROW(parser->ParseFromAttribute(L"1", b), zaf::ParseError);
    ASSERT_THROW(parser->ParseFromAttribute(L"True", b), zaf::ParseError);
    ASSERT_THROW(parser->ParseFromAttribute(L"False", b), zaf::ParseError);
    ASSERT_THROW(parser->ParseFromAttribute(L"nothing", b), zaf::ParseError);
}


TEST(BooleanTest, ParseNode) {

    auto xaml = LR"(<Boolean></Boolean>)";
    ASSERT_THROW(zaf::CreateObjectFromXaml<zaf::Boolean>(xaml), zaf::ParseError);

    xaml = LR"(<Boolean><ChildNode/></Boolean>)";
    ASSERT_THROW(zaf::CreateObjectFromXaml<zaf::Boolean>(xaml), zaf::ParseError);
}


TEST(BooleanTest, ParseToInvalidObject) {

    auto parser = zaf::Boolean::StaticType()->Parser();
    zaf::dynamic::Object object;

    ASSERT_THROW(parser->ParseFromAttribute(L"true", object), zaf::InvalidTypeError);

    auto xaml_node = zaf::XamlReader::FromString(L"<Boolean>true</Boolean>")->Read();
    ASSERT_THROW(parser->ParseFromNode(*xaml_node, object), zaf::InvalidTypeError);
} 