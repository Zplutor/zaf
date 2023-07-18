#include <gtest/gtest.h>
#include <zaf/base/auto_reset.h>

static_assert(!std::is_copy_constructible_v<zaf::AutoReset<int>>);
static_assert(!std::is_copy_assignable_v<zaf::AutoReset<int>>);
static_assert(std::is_move_constructible_v<zaf::AutoReset<int>>);
static_assert(std::is_move_assignable_v<zaf::AutoReset<int>>);

TEST(AutoResetTest, AutoReset) {

    int value{ 3 };
    {
        zaf::AutoReset auto_reset{ value };
        value = 2;
    }
    ASSERT_EQ(value, 3);
}


TEST(AutoResetTest, MoveConstruction) {

    int value{ 4 };
    {
        zaf::AutoReset auto_reset{ value };
        value = 5;
        {
            zaf::AutoReset auto_reset2 = std::move(auto_reset);
            value = 6;
        }
        ASSERT_EQ(value, 4);
        value = 7;
    }
    ASSERT_EQ(value, 7);
}


TEST(AutoResetTest, MoveAssignment) {

    int value{ 11 };
    int value2{ 1 };
    {
        zaf::AutoReset auto_reset{ value };
        value = 12;

        zaf::AutoReset auto_reset2{ value2 };
        value2 = 2;

        auto_reset = std::move(auto_reset2);
        ASSERT_EQ(value, 11);
        ASSERT_EQ(value2, 2);
    }
    ASSERT_EQ(value2, 1);
}


TEST(AutoResetTest, MakeAutoResetWithoutNewValue) {

    int value{ 8 };
    {
        auto auto_rest = zaf::MakeAutoReset(value);
        ASSERT_EQ(value, 8);
        value = 9;
    }
    ASSERT_EQ(value, 8);
}


TEST(AutoResetTest, MakeAutoResetWithNewValue) {

    int value{ 8 };
    {
        auto auto_rest = zaf::MakeAutoReset(value, 10);
        ASSERT_EQ(value, 10);
    }
    ASSERT_EQ(value, 8);
}