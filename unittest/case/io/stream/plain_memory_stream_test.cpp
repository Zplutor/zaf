#include <gtest/gtest.h>
#include <zaf/base/error/com_error.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/io/stream/stream.h>

using namespace zaf;

TEST(PlainMemoryStreamTest, CreateOnConstMemory) {

    const void* memory{};
    ASSERT_THROW(Stream::CreateOnMemory(memory, 1), PreconditionError);

    const std::byte* content = reinterpret_cast<const std::byte*>("01234");

    auto stream = Stream::CreateOnMemory(content, 0);
    ASSERT_TRUE(stream);
    ASSERT_EQ(stream.Size(), 0);
    ASSERT_EQ(stream.Position(), 0);
    ASSERT_FALSE(stream.CanWrite());
    //The underlying buffer points to the original content even if the size is zero.
    ASSERT_EQ(stream.UnderlyingBuffer(), content);

    stream = Stream::CreateOnMemory(content, 1);
    ASSERT_TRUE(stream);
    ASSERT_EQ(stream.Size(), 1);
    ASSERT_EQ(stream.Position(), 0);
    ASSERT_FALSE(stream.CanWrite());
    ASSERT_EQ(stream.UnderlyingBuffer(), content);
}


TEST(PlainMemoryStreamTest, CreateOnMutableMemory) {

    void* memory{};
    ASSERT_THROW(Stream::CreateOnMemory(memory, 1), PreconditionError);

    std::string string("01234");
    void* content = string.data();

    auto stream = Stream::CreateOnMemory(content, 0);
    ASSERT_TRUE(stream);
    ASSERT_EQ(stream.Size(), 0);
    ASSERT_EQ(stream.Position(), 0);
    ASSERT_TRUE(stream.CanWrite());
    //The underlying buffer points to the original content even if the size is zero.
    ASSERT_EQ(stream.UnderlyingBuffer(), content);

    stream = Stream::CreateOnMemory(content, 1);
    ASSERT_TRUE(stream);
    ASSERT_EQ(stream.Size(), 1);
    ASSERT_EQ(stream.Position(), 0);
    ASSERT_TRUE(stream.CanWrite());
    ASSERT_EQ(stream.UnderlyingBuffer(), content);
}


TEST(PlainMemoryStreamTest, GetPosition) {

    std::string memory("StreamTest.GetPosition");
    auto stream = zaf::Stream::CreateOnMemory(memory.data(), memory.size());
    ASSERT_EQ(stream.Position(), 0);

    auto buffer = std::make_unique<std::byte[]>(1);
    stream.Read(1, buffer.get());
    ASSERT_EQ(stream.Position(), 1);

    buffer = std::make_unique<std::byte[]>(3);
    stream.Read(3, buffer.get());
    ASSERT_EQ(stream.Position(), 4);

    stream.Seek(zaf::SeekOrigin::Begin, 2);
    ASSERT_EQ(stream.Position(), 2);

    stream.Seek(zaf::SeekOrigin::Current, 3);
    ASSERT_EQ(stream.Position(), 5);

    stream.Seek(zaf::SeekOrigin::End, 0);
    ASSERT_EQ(stream.Position(), 22);
}


TEST(PlainMemoryStreamTest, Read) {

    std::string memory{ "FromMemoryNotOwnFunction" };

    auto stream = zaf::Stream::CreateOnMemory(memory.data(), memory.size());

    //Test get length
    ASSERT_EQ(stream.Size(), memory.length());

    char buffer[10]{};
    auto read_size = stream.Read(10, buffer);
    ASSERT_EQ(read_size, 10);
    ASSERT_EQ(std::memcmp(buffer, "FromMemory", read_size), 0);

    read_size = stream.Read(6, buffer);
    ASSERT_EQ(read_size, 6);
    ASSERT_EQ(std::memcmp(buffer, "NotOwn", read_size), 0);

    read_size = stream.Read(100, buffer);
    ASSERT_EQ(read_size, 8);
    ASSERT_EQ(std::memcmp(buffer, "Function", read_size), 0);

    read_size = stream.Read(10, buffer);
    ASSERT_EQ(read_size, 0);
}


TEST(PlainMemoryStreamTest, Seek) {

    std::string memory{ "FromMemoryNotOwn_Seek" };
    auto stream = zaf::Stream::CreateOnMemory(memory.data(), memory.size());

    auto new_seek = stream.Seek(zaf::SeekOrigin::Begin, 5);
    ASSERT_EQ(new_seek, 5);
    char buffer[10]{};
    auto read_size = stream.Read(4, buffer);
    ASSERT_EQ(read_size, 4);
    ASSERT_EQ(std::memcmp(buffer, "emor", read_size), 0);

    new_seek = stream.Seek(zaf::SeekOrigin::Current, -3);
    ASSERT_EQ(new_seek, 6);
    read_size = stream.Read(4, buffer);
    ASSERT_EQ(read_size, 4);
    ASSERT_EQ(std::memcmp(buffer, "mory", read_size), 0);

    new_seek = stream.Seek(zaf::SeekOrigin::End, -9);
    ASSERT_EQ(new_seek, 12);
    read_size = stream.Read(4, buffer);
    ASSERT_EQ(read_size, 4);
    ASSERT_EQ(std::memcmp(buffer, "tOwn", read_size), 0);

    //Seek to end
    new_seek = stream.Seek(zaf::SeekOrigin::Begin, 100);
    ASSERT_EQ(new_seek, memory.length());

    new_seek = stream.Seek(zaf::SeekOrigin::Current, 100);
    ASSERT_EQ(new_seek, memory.length());

    new_seek = stream.Seek(zaf::SeekOrigin::End, 100);
    ASSERT_EQ(new_seek, memory.length());

    //Exception
    ASSERT_THROW(stream.Seek(zaf::SeekOrigin::Begin, -30), COMError);
    ASSERT_THROW(stream.Seek(zaf::SeekOrigin::Current, -30), COMError);
    ASSERT_THROW(stream.Seek(zaf::SeekOrigin::End, -30), COMError);
}


TEST(PlainMemoryStreamTest, WriteToConstMemory) {

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
    ASSERT_EQ(stream.Position(), 5);
    ASSERT_EQ(memory, "WRITEToMutableMemory");

    //Write content that beyond the length of the memory.
    std::string_view content = "TOMUTABLEMEMORY-!!";
    ASSERT_EQ(stream.Write(content.data(), content.size()), 15);
    ASSERT_EQ(stream.Position(), 20);
    ASSERT_EQ(stream.Size(), 20);
    ASSERT_EQ(memory, "WRITETOMUTABLEMEMORY");

    //Write more content to the end of the stream.
    ASSERT_EQ(stream.Write("0", 0), 0);
    ASSERT_THROW(stream.Write("1", 1), COMError);
    ASSERT_EQ(stream.Position(), 20);
    ASSERT_EQ(stream.Size(), 20);
    ASSERT_EQ(memory, "WRITETOMUTABLEMEMORY");
}