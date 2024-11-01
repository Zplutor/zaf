#include <gtest/gtest.h>
#include <zaf/graphic/rect.h>

using namespace zaf;

TEST(RectTest, Contain) {

    ASSERT_FALSE(zaf::Rect(0, 0, 0, 0).Contains(zaf::Point(0, 0)));
    ASSERT_FALSE(zaf::Rect(0, 0, 0, 0).Contains(zaf::Point(1, 1)));
    ASSERT_FALSE(zaf::Rect(0, 0, 0, 0).Contains(zaf::Point(-1, -1)));

    ASSERT_TRUE(zaf::Rect(0, 0, 10, 10).Contains(zaf::Point(0, 0)));
    ASSERT_TRUE(zaf::Rect(0, 0, 10, 10).Contains(zaf::Point(5, 0)));
    ASSERT_TRUE(zaf::Rect(0, 0, 10, 10).Contains(zaf::Point(0, 5)));

    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contains(zaf::Point(10, 0)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contains(zaf::Point(0, 10)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contains(zaf::Point(10, 5)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contains(zaf::Point(5, 10)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contains(zaf::Point(10, 10)));

    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contains(zaf::Point(-1, 0)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contains(zaf::Point(0, -1)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contains(zaf::Point(-1, -1)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contains(zaf::Point(5, -5)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contains(zaf::Point(-5, 5)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contains(zaf::Point(11, 11)));
}


TEST(RectTest, Intersect) {

    auto test = [](const zaf::Rect& intersected_rect, const zaf::Rect& expected_rect) {
        zaf::Rect primary_rect(0, 0, 10, 10);
        primary_rect.Intersect(intersected_rect);
        return primary_rect == expected_rect;
    };

    ASSERT_TRUE(test(zaf::Rect(-5, -5, 3, 3), zaf::Rect()));
    ASSERT_TRUE(test(zaf::Rect(-5, -5, 5, 5), zaf::Rect()));
    ASSERT_TRUE(test(zaf::Rect(0, -5, 5, 5), zaf::Rect()));
    ASSERT_TRUE(test(zaf::Rect(-5, 0, 5, 5), zaf::Rect()));
    ASSERT_TRUE(test(zaf::Rect(11, 11, 3, 2), zaf::Rect()));
    ASSERT_TRUE(test(zaf::Rect(10, 0, 3, 2), zaf::Rect()));
    ASSERT_TRUE(test(zaf::Rect(0, 10, 3, 2), zaf::Rect()));

    ASSERT_TRUE(test(zaf::Rect(0, 0, 3, 2), zaf::Rect(0, 0, 3, 2)));
    ASSERT_TRUE(test(zaf::Rect(4, 2, 6, 7), zaf::Rect(4, 2, 6, 7)));
    ASSERT_TRUE(test(zaf::Rect(-1, -1, 12, 12), zaf::Rect(0, 0, 10, 10)));

    ASSERT_TRUE(test(zaf::Rect(-3, -3, 6, 6), zaf::Rect(0, 0, 3, 3)));
    ASSERT_TRUE(test(zaf::Rect(5, 5, 8, 8), zaf::Rect(5, 5, 5, 5)));
}


TEST(RectTest, Union) {

    const Rect origin_rect{ 0, 0, 10, 10 };

    //Same rect
    ASSERT_EQ(Rect::Union(origin_rect, origin_rect), origin_rect);

    //Rects that have intersection
    ASSERT_EQ(Rect::Union(origin_rect, Rect(-5, -5, 10, 10)), Rect(-5, -5, 15, 15));
    ASSERT_EQ(Rect::Union(origin_rect, Rect(15, 15, 10, 10)), Rect(0, 0, 25, 25));
    ASSERT_EQ(Rect::Union(origin_rect, Rect(-10, -10, 30, 30)), Rect(-10, -10, 30, 30));
    ASSERT_EQ(Rect::Union(origin_rect, Rect(2, 2, 3, 4)), origin_rect);

    //Rects that have no intersection
    ASSERT_EQ(Rect::Union(origin_rect, Rect(-20, -20, 10, 10)), Rect(-20, -20, 30, 30));
    ASSERT_EQ(Rect::Union(origin_rect, Rect(-20, 30, 10, 10)), Rect(-20, 0, 30, 40));
    ASSERT_EQ(Rect::Union(origin_rect, Rect(15, 15, 10, 10)), Rect(0, 0, 25, 25));
    ASSERT_EQ(Rect::Union(origin_rect, Rect(-5, -20, 20, 10)), Rect(-5, -20, 20, 30));
}


TEST(RectTest, Subtract) {

    const Rect origin_rect(0, 0, 10, 10);

    ASSERT_EQ(Rect::Subtract(origin_rect, Rect(-5, -5, 3, 3)), origin_rect);
    ASSERT_EQ(Rect::Subtract(origin_rect, Rect(15, 15, 3, 3)), origin_rect);
    ASSERT_EQ(Rect::Subtract(origin_rect, Rect(11, 0, 10, 10)), origin_rect);

    ASSERT_EQ(Rect::Subtract(origin_rect, Rect(4, 4, 2, 2)), origin_rect);
    ASSERT_EQ(Rect::Subtract(origin_rect, Rect(-5, -5, 10, 10)), origin_rect);
    ASSERT_EQ(Rect::Subtract(origin_rect, Rect(-5, 5, 10, 10)), origin_rect);
    ASSERT_EQ(Rect::Subtract(origin_rect, Rect(5, -5, 10, 10)), origin_rect);
    ASSERT_EQ(Rect::Subtract(origin_rect, Rect(5, 5, 10, 10)), origin_rect);

    ASSERT_EQ(Rect::Subtract(origin_rect, Rect(-3, -3, 13, 8)), Rect(0, 5, 10, 5));
    ASSERT_EQ(Rect::Subtract(origin_rect, Rect(-2, 7, 22, 12)), Rect(0, 0, 10, 7));
    ASSERT_EQ(Rect::Subtract(origin_rect, Rect(-4, -4, 10, 18)), Rect(6, 0, 4, 10));
    ASSERT_EQ(Rect::Subtract(origin_rect, Rect(4, -4, 10, 18)), Rect(0, 0, 4, 10));

    ASSERT_EQ(Rect::Subtract(origin_rect, origin_rect), Rect());
    ASSERT_EQ(Rect::Subtract(origin_rect, Rect(-5, -5, 15, 15)), Rect());

    ASSERT_EQ(Rect::Subtract(origin_rect, Rect(0, -5, 10, 5)), origin_rect);
    ASSERT_EQ(Rect::Subtract(origin_rect, Rect(0, 10, 10, 5)), origin_rect);
    ASSERT_EQ(Rect::Subtract(origin_rect, Rect(-5, 0, 5, 10)), origin_rect);
    ASSERT_EQ(Rect::Subtract(origin_rect, Rect(10, 0, 5, 10)), origin_rect);
}


TEST(RectTest, InflateWithFrame) {

    auto test = [](const zaf::Frame& frame, const zaf::Rect& expected_rect) {
        zaf::Rect rect(0, 0, 5, 5);
        rect.Inflate(frame);
        return rect == expected_rect;
    };

    ASSERT_TRUE(test(zaf::Frame(0, 0, 0, 0), zaf::Rect(0, 0, 5, 5)));
    ASSERT_TRUE(test(zaf::Frame(1, 2, 3, 4), zaf::Rect(-1, -2, 9, 11)));
}


TEST(RectTest, DeflateWithFrame) {

    auto test = [](const zaf::Frame& frame, const zaf::Rect& expected_rect) {
        zaf::Rect rect(0, 0, 10, 10);
        rect.Deflate(frame);
        return rect == expected_rect;
    };

    ASSERT_TRUE(test(zaf::Frame(0, 0, 0, 0), zaf::Rect(0, 0, 10, 10)));
    ASSERT_TRUE(test(zaf::Frame(1, 2, 3, 4), zaf::Rect(1, 2, 6, 4)));
}


TEST(RectTest, Hash) {

    Rect rect1{ 9, 9, 99, 99 };
    Rect rect2{ 9, 9, 99, 99 };
    ASSERT_EQ(rect1.Hash(), rect2.Hash());
}


TEST(RectTest, IsEqual) {

    Rect rect1{ 9, 9, 99, 99 };
    Rect rect2{ 9, 9, 99, 99 };
    ASSERT_TRUE(rect1.IsEqual(rect2));

    Rect rect3{ 7, 8, 89, 83 };
    ASSERT_FALSE(rect1.IsEqual(rect3));
}


TEST(RectTest, ToString) {

    Rect rect{ 500, 400, 640, 480 };
    ASSERT_EQ(rect.ToString(), L"{500,400},{640,480}");
}