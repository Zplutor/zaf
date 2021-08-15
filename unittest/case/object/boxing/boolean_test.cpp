#include <gtest/gtest.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/object/boxing/boolean.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/helpers.h>
#include <zaf/object/parsing/object_parser.h>
#include "utility/assert.h"

TEST(BooleanTest, ParseAttribute) {

    auto parser = zaf::Boolean::Type->GetParser();

    zaf::Boolean b;
    parser->ParseFromAttribute(L"true", b);
    ASSERT_EQ(b.Value(), true);

    parser->ParseFromAttribute(L"false", b);
    ASSERT_EQ(b.Value(), false);

    ASSERT_THROW_ERRC(parser->ParseFromAttribute(L"0", b), zaf::BasicErrc::InvalidValue);
    ASSERT_THROW_ERRC(parser->ParseFromAttribute(L"1", b), zaf::BasicErrc::InvalidValue);
    ASSERT_THROW_ERRC(parser->ParseFromAttribute(L"True", b), zaf::BasicErrc::InvalidValue);
    ASSERT_THROW_ERRC(parser->ParseFromAttribute(L"False", b), zaf::BasicErrc::InvalidValue);
    ASSERT_THROW_ERRC(parser->ParseFromAttribute(L"nothing", b), zaf::BasicErrc::InvalidValue);
}


TEST(BooleanTest, ParseNode) {

    auto xaml = LR"(<Boolean></Boolean>)";
    ASSERT_THROW_ERRC(
        zaf::CreateObjectFromXaml<zaf::Boolean>(xaml),
        zaf::BasicErrc::InvalidValue);

    xaml = LR"(<Boolean><ChildNode/></Boolean>)";
    ASSERT_THROW_ERRC(
        zaf::CreateObjectFromXaml<zaf::Boolean>(xaml),
        zaf::BasicErrc::InvalidValue);
}