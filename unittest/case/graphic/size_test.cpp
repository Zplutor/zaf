#include <gtest/gtest.h>
#include <zaf/graphic/size.h>

TEST(SizeTest, Hash) {

    zaf::Size size1{ 10, 20 };
    zaf::Size size2{ 10, 20 };
    ASSERT_EQ(size1.Hash(), size2.Hash());
}


TEST(SizeTest, IsEqual) {

    zaf::Size size1{ 11, 22 };
    zaf::Size size2{ 11, 22 };
    ASSERT_TRUE(size1.IsEqual(size2));

    zaf::Size size3{ 33, 44 };
    ASSERT_FALSE(size1.IsEqual(size3));
}


TEST(SizeTest, ToString) {

    zaf::Size size{ 132, 444 };
    ASSERT_EQ(size.ToString(), L"132,444");
}