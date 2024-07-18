#include <gtest/gtest.h>
#include <zaf/graphic/rounded_rect.h>

TEST(RoundedRectTest, Hash) {

    zaf::RoundedRect rect1{ zaf::Rect{ 0, 0, 10, 20 }, 2, 3 };
    zaf::RoundedRect rect2{ zaf::Rect{ 0, 0, 10, 20 }, 2, 3 };
    ASSERT_EQ(rect1.Hash(), rect2.Hash());
}


TEST(RoundedRectTest, IsEqual) {

    zaf::RoundedRect rect1{ zaf::Rect{ 0, 0, 10, 20 }, 2, 3 };
    zaf::RoundedRect rect2{ zaf::Rect{ 0, 0, 10, 20 }, 2, 3 };
    ASSERT_TRUE(rect1.IsEqual(rect2));

    zaf::RoundedRect rect3{ zaf::Rect{ 1, 1, 20, 30 }, 1, 1 };
    ASSERT_FALSE(rect1.IsEqual(rect3));
}


TEST(RoundedRectTest, ToString) {

    zaf::RoundedRect rect{ zaf::Rect{ 50, 60, 70, 77 }, 2, 3 };
    ASSERT_EQ(rect.ToString(), L"{{50,60},{70,77}},2,3");
}