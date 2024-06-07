#include <gtest/gtest.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/io/stream/stream.h>

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


TEST(ByteArrayStreamTest, Write) {

    auto stream = Stream::FromMemory(0);
    ASSERT_EQ(stream.Write("01234", 5), 5);

    std::string_view actual{ 
        reinterpret_cast<const char*>(stream.GetUnderlyingBuffer()), 
        stream.GetSize()
    };
    ASSERT_EQ(actual, "01234");
}