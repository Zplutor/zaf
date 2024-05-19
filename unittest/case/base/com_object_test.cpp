#include <gtest/gtest.h>
#include <zaf/base/com_object.h>
#include <zaf/base/stream.h>

TEST(COMObjectTest, ConversionFromNullptr) {

    //Explicit construct from a nullptr.
    zaf::Stream object{ nullptr };
    ASSERT_FALSE(object);

    //Implicit construct from a nullptr.
    auto function = [](zaf::Stream stream) {
        return !!stream;
    };
    ASSERT_FALSE(function(nullptr));

    //Assign from a nullptr.
    object = zaf::Stream::FromMemory("a", 1);
    ASSERT_TRUE(object);
    object = nullptr;
    ASSERT_FALSE(object);
}


TEST(COMObjectTest, CheckNull) {

    zaf::Stream object;
    ASSERT_FALSE(object);
    ASSERT_TRUE(!object);
    ASSERT_TRUE(object == nullptr);
    ASSERT_FALSE(object != nullptr);
    ASSERT_TRUE(nullptr == object);
    ASSERT_FALSE(nullptr != object);

    object = zaf::Stream::FromMemory("a", 1);
    ASSERT_TRUE(object);
    ASSERT_FALSE(!object);
    ASSERT_FALSE(object == nullptr);
    ASSERT_TRUE(object != nullptr);
    ASSERT_FALSE(nullptr == object);
    ASSERT_TRUE(nullptr != object);
}