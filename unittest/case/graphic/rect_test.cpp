#include <gtest/gtest.h>
#include <zaf/graphic/rect.h>

TEST(TestRect, Contain) {

    ASSERT_FALSE(zaf::Rect(0, 0, 0, 0).Contain(zaf::Point(0, 0)));
    ASSERT_FALSE(zaf::Rect(0, 0, 0, 0).Contain(zaf::Point(1, 1)));
    ASSERT_FALSE(zaf::Rect(0, 0, 0, 0).Contain(zaf::Point(-1, -1)));

    ASSERT_TRUE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(0, 0)));
    ASSERT_TRUE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(5, 0)));
    ASSERT_TRUE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(0, 5)));

    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(10, 0)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(0, 10)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(10, 5)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(5, 10)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(10, 10)));

    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(-1, 0)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(0, -1)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(-1, -1)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(5, -5)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(-5, 5)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(11, 11)));
}


TEST(TestRect, Intersect) {

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