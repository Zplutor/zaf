#include <gtest/gtest.h>
#include <zaf/graphic/frame.h>

TEST(FrameTest, Hash) {

    zaf::Frame frame1{ 1, 2, 3, 4 };
    zaf::Frame frame2{ 1, 2, 3, 4 };
    ASSERT_EQ(frame1.Hash(), frame2.Hash());
}


TEST(FrameTest, IsEqual) {

    zaf::Frame frame1{ 1, 2, 3, 4 };
    zaf::Frame frame2{ 1, 2, 3, 4 };
    ASSERT_TRUE(frame1.IsEqual(frame2));

    zaf::Frame frame3{ 5, 6, 7, 8 };
    ASSERT_FALSE(frame1.IsEqual(frame3));
}


TEST(FrameTest, ToString) {

    zaf::Frame frame{ 7, 3, 99, 11 };
    ASSERT_EQ(frame.ToString(), L"7,3,99,11");
}