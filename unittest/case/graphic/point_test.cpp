#include <gtest/gtest.h>
#include <zaf/graphic/point.h>

TEST(PointTest, Hash) {

    zaf::Point point1{ 11, -1 };
    zaf::Point point2{ 11, -1 };
    ASSERT_EQ(point1.Hash(), point2.Hash());
}


TEST(PointTest, IsEqual) {

    zaf::Point point1{ 11, -1 };
    zaf::Point point2{ 11, -1 };
    ASSERT_TRUE(point1.IsEqual(point2));

    zaf::Point point3{ 11, -2 };
    ASSERT_FALSE(point1.IsEqual(point3));
}


TEST(PointTest, ToString) {

    zaf::Point point{ 0, -10 };
    ASSERT_EQ(point.ToString(), L"0,-10");
}


TEST(PointTest, CloneFrom) {

    zaf::Point point;
    ASSERT_THROW(point.CloneFrom({}), zaf::InvalidTypeError);

    point.CloneFrom(zaf::Point{ 101.f, 102.f });
    ASSERT_EQ(point.x, 101.f);
    ASSERT_EQ(point.y, 102.f);
}