#include <gtest/gtest.h>
#include <zaf/graphic/pixel_snapping.h>

using namespace zaf;

TEST(AlignmentTest, ToPixelAlignedInPixels_Point) {

    ASSERT_EQ(SnapAndTransformToPixels(Point(10, 10), 96), Point(10, 10));
    ASSERT_EQ(SnapAndTransformToPixels(Point(10, 10), 1, 96), Point(10.5, 10.5));
    ASSERT_EQ(SnapAndTransformToPixels(Point(0.4f, 0.4f), 0, 96), Point(0, 0));
    ASSERT_EQ(SnapAndTransformToPixels(Point(0.4f, 0.4f), 1, 96), Point(0.5f, 0.5f));
    ASSERT_EQ(SnapAndTransformToPixels(Point(0.6f, 0.6f), 0, 96), Point(1, 1));
    ASSERT_EQ(SnapAndTransformToPixels(Point(0.6f, 0.6f), 1, 96), Point(1.5f, 1.5f));

    ASSERT_EQ(SnapAndTransformToPixels(Point(10, 10), 192), Point(20, 20));
    ASSERT_EQ(SnapAndTransformToPixels(Point(10, 10), 1.f, 192), Point(21, 21));
    ASSERT_EQ(SnapAndTransformToPixels(Point(0.4f, 0.4f), 0, 192), Point(1, 1));
    ASSERT_EQ(SnapAndTransformToPixels(Point(0.4f, 0.4f), 1, 192), Point(2, 2));
    ASSERT_EQ(SnapAndTransformToPixels(Point(0.6f, 0.6f), 0, 192), Point(1, 1));
    ASSERT_EQ(SnapAndTransformToPixels(Point(0.6f, 0.6f), 1, 192), Point(2, 2));
}


TEST(AlignmentTest, ToPixelAlignedInPixels_Rect) {

    ASSERT_EQ(SnapAndTransformToPixels(Rect(10, 10, 10, 10), 96), Rect(10, 10, 10, 10));
    ASSERT_EQ(SnapAndTransformToPixels(Rect(10, 10, 10, 10), 1, 96), Rect(10.5, 10.5, 9, 9));
    ASSERT_EQ(SnapAndTransformToPixels(Rect(0.4f, 0.4f, 0.4f, 0.4f), 0, 96), Rect(0, 0, 1, 1));
    ASSERT_EQ(SnapAndTransformToPixels(Rect(0.4f, 0.4f, 0.4f, 0.4f), 1, 96), Rect(0.5f, 0.5f, 0, 0));
    ASSERT_EQ(SnapAndTransformToPixels(Rect(0.6f, 0.6f, 0.6f, 0.6f), 0, 96), Rect(1, 1, 0, 0));
    ASSERT_EQ(SnapAndTransformToPixels(Rect(0.6f, 0.6f, 0.6f, 0.6f), 1, 96), Rect(1.5f, 1.5f, 0, 0));

    ASSERT_EQ(SnapAndTransformToPixels(Rect(10, 10, 10, 10), 192), Rect(20, 20, 20, 20));
    ASSERT_EQ(SnapAndTransformToPixels(Rect(10, 10, 10, 10), 1, 192), Rect(21, 21, 18, 18));
    ASSERT_EQ(SnapAndTransformToPixels(Rect(0.4f, 0.4f, 0.4f, 0.4f), 0, 192), Rect(1, 1, 1, 1));
    ASSERT_EQ(SnapAndTransformToPixels(Rect(0.4f, 0.4f, 0.4f, 0.4f), 1, 192), Rect(2, 2, 0, 0));
    ASSERT_EQ(SnapAndTransformToPixels(Rect(0.6f, 0.6f, 0.6f, 0.6f), 0, 192), Rect(1, 1, 1, 1));
    ASSERT_EQ(SnapAndTransformToPixels(Rect(0.6f, 0.6f, 0.6f, 0.6f), 1, 192), Rect(2, 2, 0, 0));
}


TEST(AlignmentTest, ToPixelAligned_Point) {

    ASSERT_EQ(SnapToPixels(Point(10, 10), 96), Point(10, 10));
    ASSERT_EQ(SnapToPixels(Point(10, 10), 1, 96), Point(10.5, 10.5));
    ASSERT_EQ(SnapToPixels(Point(0.4f, 0.4f), 0, 96), Point(0, 0));
    ASSERT_EQ(SnapToPixels(Point(0.4f, 0.4f), 1, 96), Point(0.5f, 0.5f));
    ASSERT_EQ(SnapToPixels(Point(0.6f, 0.6f), 0, 96), Point(1, 1));
    ASSERT_EQ(SnapToPixels(Point(0.6f, 0.6f), 1, 96), Point(1.5f, 1.5f));

    ASSERT_EQ(SnapToPixels(Point(10, 10), 192), Point(10, 10));
    ASSERT_EQ(SnapToPixels(Point(10, 10), 1.f, 192), Point(10.5, 10.5));
    ASSERT_EQ(SnapToPixels(Point(0.4f, 0.4f), 0, 192), Point(0.5f, 0.5f));
    ASSERT_EQ(SnapToPixels(Point(0.4f, 0.4f), 1, 192), Point(1, 1));
    ASSERT_EQ(SnapToPixels(Point(0.6f, 0.6f), 0, 192), Point(0.5, 0.5));
    ASSERT_EQ(SnapToPixels(Point(0.6f, 0.6f), 1, 192), Point(1, 1));
}


TEST(AlignmentTest, ToPixelAligned_Rect) {

    ASSERT_EQ(SnapToPixels(Rect(10, 10, 10, 10), 96), Rect(10, 10, 10, 10));
    ASSERT_EQ(SnapToPixels(Rect(10, 10, 10, 10), 1, 96), Rect(10.5, 10.5, 9, 9));
    ASSERT_EQ(SnapToPixels(Rect(0.4f, 0.4f, 0.4f, 0.4f), 0, 96), Rect(0, 0, 1, 1));
    ASSERT_EQ(SnapToPixels(Rect(0.4f, 0.4f, 0.4f, 0.4f), 1, 96), Rect(0.5f, 0.5f, 0, 0));
    ASSERT_EQ(SnapToPixels(Rect(0.6f, 0.6f, 0.6f, 0.6f), 0, 96), Rect(1, 1, 0, 0));
    ASSERT_EQ(SnapToPixels(Rect(0.6f, 0.6f, 0.6f, 0.6f), 1, 96), Rect(1.5f, 1.5f, 0, 0));

    ASSERT_EQ(SnapToPixels(Rect(10, 10, 10, 10), 192), Rect(10, 10, 10, 10));
    ASSERT_EQ(SnapToPixels(Rect(10, 10, 10, 10), 1, 192), Rect(10.5, 10.5, 9, 9));
    ASSERT_EQ(SnapToPixels(Rect(0.4f, 0.4f, 0.4f, 0.4f), 0, 192), Rect(0.5f, 0.5f, 0.5f, 0.5f));
    ASSERT_EQ(SnapToPixels(Rect(0.4f, 0.4f, 0.4f, 0.4f), 1, 192), Rect(1, 1, 0, 0));
    ASSERT_EQ(SnapToPixels(Rect(0.6f, 0.6f, 0.6f, 0.6f), 0, 192), Rect(0.5f, 0.5f, 0.5f, 0.5f));
    ASSERT_EQ(SnapToPixels(Rect(0.6f, 0.6f, 0.6f, 0.6f), 1, 192), Rect(1, 1, 0, 0));
}


TEST(AlignmentTest, ToPixelAligned_RoundedRect) {

    //96 DPI
    ASSERT_EQ(
        SnapToPixels(RoundedRect(Rect(10, 10, 10, 10), 10, 10), 96), 
        RoundedRect(Rect(10, 10, 10, 10), 10, 10));

    ASSERT_EQ(
        SnapToPixels(RoundedRect(Rect(10, 10, 10, 10), 10, 10), 1, 96),
        RoundedRect(Rect(10.5, 10.5, 9, 9), 10, 10));

    ASSERT_EQ(
        SnapToPixels(RoundedRect(Rect(0.4f, 0.4f, 10, 10), 1.4f, 1.4f), 0, 96),
        RoundedRect(Rect(0, 0, 10, 10), 1, 1));

    ASSERT_EQ(
        SnapToPixels(RoundedRect(Rect(0.4f, 0.4f, 10, 10), 1.4f, 1.4f), 1, 96),
        RoundedRect(Rect(0.5f, 0.5f, 9, 9), 1, 1));

    ASSERT_EQ(
        SnapToPixels(RoundedRect(Rect(0.6f, 0.6f, 10, 10), 2.6f, 2.6f), 0, 96),
        RoundedRect(Rect(1, 1, 10, 10), 3, 3));

    ASSERT_EQ(
        SnapToPixels(RoundedRect(Rect(0.6f, 0.6f, 10, 10), 2.6f, 2.6f), 1, 96),
        RoundedRect(Rect(1.5f, 1.5f, 9, 9), 3, 3));

    //192 DPI
    ASSERT_EQ(
        SnapToPixels(RoundedRect(Rect(10, 10, 10, 10), 10, 10), 192),
        RoundedRect(Rect(10, 10, 10, 10), 10, 10));

    ASSERT_EQ(
        SnapToPixels(RoundedRect(Rect(10, 10, 10, 10), 10, 10), 1, 192),
        RoundedRect(Rect(10.5, 10.5, 9, 9), 10, 10));

    ASSERT_EQ(
        SnapToPixels(RoundedRect(Rect(0.4f, 0.4f, 10, 10), 1.4f, 1.4f), 0, 192),
        RoundedRect(Rect(0.5, 0.5, 10, 10), 1.5f, 1.5f));

    ASSERT_EQ(
        SnapToPixels(RoundedRect(Rect(0.4f, 0.4f, 10, 10), 1.4f, 1.4f), 1, 192),
        RoundedRect(Rect(1, 1, 9, 9), 1.5f, 1.5f));

    ASSERT_EQ(
        SnapToPixels(RoundedRect(Rect(0.6f, 0.6f, 10, 10), 2.6f, 2.6f), 0, 192),
        RoundedRect(Rect(0.5f, 0.5f, 10, 10), 2.5f, 2.5f));

    ASSERT_EQ(
        SnapToPixels(RoundedRect(Rect(0.6f, 0.6f, 10, 10), 2.6f, 2.6f), 1, 192),
        RoundedRect(Rect(1, 1, 9, 9), 2.5f, 2.5f));
}