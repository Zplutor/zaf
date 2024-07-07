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


TEST(EllipseTest, CloneFrom) {

    zaf::Ellipse ellipse;
    ASSERT_THROW(ellipse.CloneFrom({}), zaf::InvalidTypeError);

    ellipse.CloneFrom(zaf::Ellipse{ 0.1f, 0.2f, 5.f, 5.f });
    ASSERT_EQ(ellipse.x_radius, 5.f);
    ASSERT_EQ(ellipse.y_radius, 5.f);
    ASSERT_EQ(ellipse.position.x, 0.1f);
    ASSERT_EQ(ellipse.position.y, 0.2f);
}