#include <gtest/gtest.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/io/stream/stream.h>

using namespace zaf;

TEST(ByteArrayStreamTest, Create_InitialSize) {

    auto stream = Stream::FromMemory(0);
    ASSERT_TRUE(stream);
    ASSERT_EQ(stream.Size(), 0);
    ASSERT_EQ(stream.Position(), 0);
    ASSERT_TRUE(stream.CanWrite());
    //The underlying buffer is null if the size is 0.
    ASSERT_EQ(stream.UnderlyingBuffer(), nullptr);

    stream = Stream::FromMemory(1);
    ASSERT_TRUE(stream);
    ASSERT_EQ(stream.Size(), 1);
    ASSERT_EQ(stream.Position(), 0);
    ASSERT_TRUE(stream.CanWrite());
    ASSERT_NE(stream.UnderlyingBuffer(), nullptr);
}


TEST(ByteArrayStreamTest, Create_CopyMemory) {

    ASSERT_THROW(Stream::FromMemory(nullptr, 1), PreconditionError);

    auto stream = Stream::FromMemory("0", 0);
    ASSERT_TRUE(stream);
    ASSERT_EQ(stream.Size(), 0);
    ASSERT_EQ(stream.Position(), 0);
    ASSERT_TRUE(stream.CanWrite());
    ASSERT_EQ(stream.UnderlyingBuffer(), nullptr);

    stream = Stream::FromMemory("123", 1);
    ASSERT_TRUE(stream);
    ASSERT_EQ(stream.Size(), 1);
    ASSERT_EQ(stream.Position(), 0);
    ASSERT_TRUE(stream.CanWrite());
    ASSERT_NE(stream.UnderlyingBuffer(), nullptr);
}


TEST(ByteArrayStreamTest, Write) {

    auto stream = Stream::FromMemory(0);
    ASSERT_EQ(stream.Write("01234", 5), 5);

    std::string_view actual{ 
        reinterpret_cast<const char*>(stream.UnderlyingBuffer()), 
        stream.Size()
    };
    ASSERT_EQ(actual, "01234");
}