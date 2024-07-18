#include <gtest/gtest.h>
#include <zaf/graphic/ellipse.h>

TEST(EllipseTest, Hash) {

    zaf::Ellipse ellipse1{ 1, 2, 10, 20 };
    zaf::Ellipse ellipse2{ 1, 2, 10, 20 };
    ASSERT_EQ(ellipse1.Hash(), ellipse2.Hash());
}


TEST(EllipseTest, IsEqual) {

    zaf::Ellipse ellipse1{ 1, 2, 10, 20 };
    zaf::Ellipse ellipse2{ 1, 2, 10, 20 };
    ASSERT_TRUE(ellipse1.IsEqual(ellipse2));

    zaf::Ellipse ellipse3{ -5, -5, 100, 20 };
    ASSERT_FALSE(ellipse1.IsEqual(ellipse3));
}


TEST(EllipseTest, ToString) {

    zaf::Ellipse ellipse{ -3, -4, 7, 8 };
    ASSERT_EQ(ellipse.ToString(), L"{-3,-4},7,8");
}