#include <gtest/gtest.h>
#include <zaf/base/flag_enum.h>

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

ZAF_ENABLE_FLAG_ENUM(Flag)

}

TEST(FlagEnumTest, IsFlagEnum) {

    ASSERT_TRUE(zaf::IsFlagEnum<Flag>::Value);
    ASSERT_FALSE(zaf::IsFlagEnum<NonFlag>::Value);
    ASSERT_FALSE(zaf::IsFlagEnum<int>::Value);
}
