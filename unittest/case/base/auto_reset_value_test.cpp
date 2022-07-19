#include <gtest/gtest.h>
#include <zaf/base/auto_reset_value.h>

TEST(AutoResetValue, Constructor) {

    zaf::AutoResetValue<int> i;
    ASSERT_EQ(i.Value(), 0);

    zaf::AutoResetValue<std::string> string{ "sss" };
    ASSERT_EQ(string.Value(), "sss");
}


TEST(AutoResetValue, AutoReset) {

    zaf::AutoResetValue<int> i{ 10 };
    
    {
        auto guard = i.BeginSet(8);
        ASSERT_EQ(i.Value(), 8);
    }

    ASSERT_EQ(i.Value(), 10);
}