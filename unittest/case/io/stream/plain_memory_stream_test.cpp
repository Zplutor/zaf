#include <gtest/gtest.h>
#include <zaf/base/error/com_error.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/stream.h>

using namespace zaf;

TEST(PlainMemoyStreamTest, CreateOnConstMemory) {

    const void* memory{};
    ASSERT_THROW(Stream::CreateOnMemory(memory, 1), PreconditionError);

    const std::byte* content = reinterpret_cast<const std::byte*>("01234");

    auto stream = Stream::CreateOnMemory(content, 0);
    ASSERT_TRUE(stream);
    ASSERT_EQ(stream.GetSize(), 0);
    ASSERT_EQ(stream.GetPosition(), 0);
    ASSERT_FALSE(stream.CanWrite());
    //The underlying buffer points to the original content even if the size is zero.
    ASSERT_EQ(stream.GetUnderlyingBuffer(), content);

    stream = Stream::CreateOnMemory(content, 1);
    ASSERT_TRUE(stream);
    ASSERT_EQ(stream.GetSize(), 1);
    ASSERT_EQ(stream.GetPosition(), 0);
    ASSERT_FALSE(stream.CanWrite());
    ASSERT_EQ(stream.GetUnderlyingBuffer(), content);
}


TEST(PlainMemoyStreamTest, CreateOnMutableMemory) {

    void* memory{};
    ASSERT_THROW(Stream::CreateOnMemory(memory, 1), PreconditionError);

    std::string string("01234");
    void* content = string.data();

    auto stream = Stream::CreateOnMemory(content, 0);
    ASSERT_TRUE(stream);
    ASSERT_EQ(stream.GetSize(), 0);
    ASSERT_EQ(stream.GetPosition(), 0);
    ASSERT_TRUE(stream.CanWrite());
    //The underlying buffer points to the original content even if the size is zero.
    ASSERT_EQ(stream.GetUnderlyingBuffer(), content);

    stream = Stream::CreateOnMemory(content, 1);
    ASSERT_TRUE(stream);
    ASSERT_EQ(stream.GetSize(), 1);
    ASSERT_EQ(stream.GetPosition(), 0);
    ASSERT_TRUE(stream.CanWrite());
    ASSERT_EQ(stream.GetUnderlyingBuffer(), content);
}


TEST(PlainMemoyStreamTest, WriteToConstMemory) {

    std::string memory{ "WriteToConstMemory" };
    auto stream = zaf::Stream::CreateOnMemory(
        static_cast<const void*>(memory.data()), 
        memory.size());

    ASSERT_THROW(stream.Write(L"something", 9), COMError);
    ASSERT_EQ(memory, "WriteToConstMemory");
}


TEST(PlainMemoryStreamTest, WriteToMutableMemory) {

    std::string memory{ "WriteToMutableMemory" };
    auto stream = Stream::CreateOnMemory(memory.data(), memory.size());

    ASSERT_EQ(stream.Write("WRITE", 5), 5);
    ASSERT_EQ(stream.GetPosition(), 5);
    ASSERT_EQ(memory, "WRITEToMutableMemory");

    //Write content that beyond the length of the memory.
    std::string_view content = "TOMUTABLEMEMORY-!!";
    ASSERT_EQ(stream.Write(content.data(), content.size()), 15);
    ASSERT_EQ(stream.GetPosition(), 20);
    ASSERT_EQ(stream.GetSize(), 20);
    ASSERT_EQ(memory, "WRITETOMUTABLEMEMORY");

    //Write more content to the end of the stream.
    ASSERT_EQ(stream.Write("0", 0), 0);
    ASSERT_THROW(stream.Write("1", 1), COMError);
    ASSERT_EQ(stream.GetPosition(), 20);
    ASSERT_EQ(stream.GetSize(), 20);
    ASSERT_EQ(memory, "WRITETOMUTABLEMEMORY");
}