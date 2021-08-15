#include <gtest/gtest.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/graphic/font/font_weight.h>
#include <zaf/object/parsing/helpers.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/object_parser.h>
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