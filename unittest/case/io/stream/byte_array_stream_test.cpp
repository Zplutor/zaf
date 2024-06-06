#include <gtest/gtest.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/stream.h>

using namespace zaf;

TEST(ByteArrayStreamTest, Create_InitialSize) {

    auto stream = Stream::FromMemory(0);
    ASSERT_TRUE(stream);
    ASSERT_EQ(stream.GetSize(), 0);
    ASSERT_EQ(stream.GetPosition(), 0);
    ASSERT_TRUE(stream.CanWrite());
    //The underlying buffer is null if the size is 0.
    ASSERT_EQ(stream.GetUnderlyingBuffer(), nullptr);

    stream = Stream::FromMemory(1);
    ASSERT_TRUE(stream);
    ASSERT_EQ(stream.GetSize(), 1);
    ASSERT_EQ(stream.GetPosition(), 0);
    ASSERT_TRUE(stream.CanWrite());
    ASSERT_NE(stream.GetUnderlyingBuffer(), nullptr);
}


TEST(ByteArrayStreamTest, Create_CopyMemory) {

    ASSERT_THROW(Stream::FromMemory(nullptr, 1), PreconditionError);

    auto stream = Stream::FromMemory("0", 0);
    ASSERT_TRUE(stream);
    ASSERT_EQ(stream.GetSize(), 0);
    ASSERT_EQ(stream.GetPosition(), 0);
    ASSERT_TRUE(stream.CanWrite());
    ASSERT_EQ(stream.GetUnderlyingBuffer(), nullptr);

    stream = Stream::FromMemory("123", 1);
    ASSERT_TRUE(stream);
    ASSERT_EQ(stream.GetSize(), 1);
    ASSERT_EQ(stream.GetPosition(), 0);
    ASSERT_TRUE(stream.CanWrite());
    ASSERT_NE(stream.GetUnderlyingBuffer(), nullptr);
}
