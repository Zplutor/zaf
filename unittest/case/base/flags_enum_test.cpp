#include <gtest/gtest.h>
#include <zaf/base/flags_enum.h>

namespace {

enum class NonFlag {
    Zero = 0,
    One = 1,
    Two = 2,
    Three = 3,
    Four = 4,
};

enum class Flag {
    Zero = 0,
    One = 1,
    Two = 2,
    Three = 4,
    Four = 8,
};

ZAF_ENABLE_FLAGS_ENUM(Flag)

}

static_assert(zaf::IsFlagsEnumV<Flag>);
static_assert(!zaf::IsFlagsEnumV<NonFlag>);
static_assert(!zaf::IsFlagsEnumV<int>);


TEST(FlagsEnumTest, BitwiseOperators) {

    ASSERT_EQ(static_cast<int>(~Flag::One), 0xfffffffe);
    ASSERT_EQ(static_cast<int>(Flag::One | Flag::Two), 3);
    ASSERT_EQ(static_cast<Flag>(0xff) & Flag::Four, Flag::Four);
    ASSERT_EQ(static_cast<Flag>(0xff) ^ Flag::Three, static_cast<Flag>(0xfb));

    Flag value = Flag::Zero;
    value |= Flag::One;
    ASSERT_EQ(value, Flag::One);

    value &= Flag::Zero;
    ASSERT_EQ(value, Flag::Zero);

    value ^= Flag::Two;
    ASSERT_EQ(value, Flag::Two);
}


TEST(FlagsEnumTest, HasFlag) {

    Flag flag{};
    ASSERT_TRUE(zaf::HasFlag(flag, Flag::Zero));
    ASSERT_FALSE(zaf::HasFlag(flag, Flag::One));
    ASSERT_FALSE(zaf::HasFlag(flag, Flag::Two));
    ASSERT_FALSE(zaf::HasFlag(flag, Flag::Three));
    ASSERT_FALSE(zaf::HasFlag(flag, Flag::Four));

    flag = Flag::One | Flag::Three;
    ASSERT_TRUE(zaf::HasFlag(flag, Flag::Zero));
    ASSERT_TRUE(zaf::HasFlag(flag, Flag::One));
    ASSERT_FALSE(zaf::HasFlag(flag, Flag::Two));
    ASSERT_TRUE(zaf::HasFlag(flag, Flag::Three));
    ASSERT_FALSE(zaf::HasFlag(flag, Flag::Four));
}