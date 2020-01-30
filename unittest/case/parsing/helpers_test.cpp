#include <gtest/gtest.h>
#include <zaf/graphic/point.h>
#include <zaf/parsing/helpers.h>

using namespace zaf;

TEST(ParsingHelpers, CreateObjectFromXaml) {

    auto point = CreateObjectFromXaml<Point>(R"(<Point X="1" Y="2" />)");
    ASSERT_NE(point, nullptr);
    ASSERT_EQ(point->x, 1);
    ASSERT_EQ(point->y, 2);

    point = CreateObjectFromXaml<Point>(LR"(<Point X="3" Y="4" />)");
    ASSERT_NE(point, nullptr);
    ASSERT_EQ(point->x, 3);
    ASSERT_EQ(point->y, 4);
}