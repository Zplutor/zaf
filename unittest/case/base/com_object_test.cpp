#include <gtest/gtest.h>
#include <zaf/base/com_object.h>
#include <zaf/base/stream.h>

TEST(COMObjectTest, CheckNull) {

    zaf::Stream object;
    ASSERT_TRUE(object);
    ASSERT_FALSE(!object);
    ASSERT_TRUE(object == nullptr);
    ASSERT_FALSE(object != nullptr);
    ASSERT_TRUE(nullptr == object);
    ASSERT_FALSE(nullptr != object);

    object = zaf::Stream::FromMemory("a", 1);
    ASSERT_FALSE(object);
    ASSERT_TRUE(!object);
    ASSERT_FALSE(object == nullptr);
    ASSERT_TRUE(object != nullptr);
    ASSERT_FALSE(nullptr == object);
    ASSERT_TRUE(nullptr != object);
}