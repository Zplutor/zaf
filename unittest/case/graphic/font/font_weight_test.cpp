#include <gtest/gtest.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/creation.h>
#include <zaf/graphic/font/font_weight.h>
#include <zaf/object/parsing/helpers.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_reader.h>
#include "utility/assert.h"

TEST(FontWeightTest, ParseInvalidAttributeValue) {

    auto parser = zaf::FontWeight::Type->GetParser();

    zaf::FontWeight font_weight;
    ASSERT_THROW_ERRC(
        parser->ParseFromAttribute(L"", font_weight), 
        zaf::BasicErrc::InvalidValue);

    ASSERT_THROW_ERRC(
        parser->ParseFromAttribute(L"3223ddsf", font_weight),
        zaf::BasicErrc::InvalidValue);
}


TEST(FontWeightTest, ParseInvalidPropertyNodeValue) {

    auto xaml = LR"(<FontWeight></FontWeight>)";
    ASSERT_THROW_ERRC(
        zaf::CreateObjectFromXaml<zaf::FontWeight>(xaml), 
        zaf::BasicErrc::InvalidValue);

    xaml = LR"(<FontWeight><ChildNode/></FontWeight>)";
    ASSERT_THROW_ERRC(
        zaf::CreateObjectFromXaml<zaf::FontWeight>(xaml),
        zaf::BasicErrc::InvalidValue);
}


TEST(FontWeightTest, ParseToInvalidObject) {

    auto parser = zaf::FontWeight::Type->GetParser();
    zaf::Object object;

    ASSERT_THROW_ERRC(parser->ParseFromAttribute(L"100", object), zaf::BasicErrc::InvalidCast);

    auto xaml_node = zaf::XamlReader::FromString(L"<FontWeight>11</FontWeight>")->Read();
    ASSERT_THROW_ERRC(parser->ParseFromNode(*xaml_node, object), zaf::BasicErrc::InvalidCast);
}