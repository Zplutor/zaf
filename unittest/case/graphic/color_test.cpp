#include <gtest/gtest.h>
#include <zaf/graphic/color.h>

TEST(ColorTest, Hash) {

    zaf::Color color1{ 0.1f, 0.2f, 0.3f, 0.4f };
    zaf::Color color2{ 0.1f, 0.2f, 0.3f, 0.4f };
    ASSERT_EQ(color1.Hash(), color2.Hash());
}


TEST(ColorTest, IsEqual) {

    zaf::Color color1{ 0.1f, 0.2f, 0.3f, 0.4f };
    zaf::Color color2{ 0.1f, 0.2f, 0.3f, 0.4f };
    ASSERT_TRUE(color1.IsEqual(color2));

    zaf::Color color3{ 0.4f, 0.2f, 0.3f, 0.4f };
    ASSERT_FALSE(color1.IsEqual(color3));
}


TEST(ColorTest, ToString) {

    zaf::Color color{ 0.1f, 0.2f, 0.3f, 0.4f };
    ASSERT_EQ(color.ToString(), L"0.1,0.2,0.3,0.4");
}