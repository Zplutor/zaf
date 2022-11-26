#include <gtest/gtest.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/graphic/size.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/parsing/xaml_reader.h>
#include "utility/assert.h"

TEST(SizeParserTest, ParseFromAttribute) {

    zaf::Size size;
    size.GetType()->GetParser()->ParseFromAttribute(L"100,101", size);

    ASSERT_EQ(size, zaf::Size(100, 101));
}


TEST(SizeParserTest, ParseFromNode) {

    auto xaml = R"(
        <Size Width="100" Height="101" />
    )";
    auto node = zaf::XamlReader::FromString(xaml)->Read();

    zaf::Size size;
    auto parser = zaf::Size::Type->GetParser();
    parser->ParseFromNode(*node, size);
    ASSERT_EQ(size, zaf::Size(100, 101));

    xaml = "<Size><Size.Width>102</Size.Width><Size.Height>103</Size.Height></Size>";
    node = zaf::XamlReader::FromString(xaml)->Read();

    parser->ParseFromNode(*node, size);
    ASSERT_EQ(size, zaf::Size(102, 103));
}


TEST(SizeParserTest, ParseToInvalidObject) {

    auto parser = zaf::Size::Type->GetParser();
    zaf::Object object;

    ASSERT_THROW_ERRC(parser->ParseFromAttribute(L"400,400", object), zaf::BasicErrc::InvalidCast);
}


TEST(SizeParserTest, ParseInvalidValue) {

    auto parser = zaf::Size::Type->GetParser();

    zaf::Size size;
    ASSERT_THROW_ERRC(parser->ParseFromAttribute(L"ab,cd", size), zaf::BasicErrc::InvalidValue);
}