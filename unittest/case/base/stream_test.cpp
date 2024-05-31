#include <gtest/gtest.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/base/error/error.h>
#include <zaf/base/stream.h>

using namespace zaf;

TEST(StreamTest, FromMemory_Precondition) {
    ASSERT_THROW(Stream::FromMemory(nullptr, 1), PreconditionError);
}


TEST(StreamTest, FromMemoryNoCopy_Precondition) {
    ASSERT_THROW(Stream::FromMemoryNoCopy(nullptr, 1), PreconditionError);
}


TEST(StreamTest, GetPosition) {

    std::string memory("StreamTest.GetPosition");
    auto stream = zaf::Stream::FromMemory(memory.data(), memory.size());
    ASSERT_EQ(stream.GetPosition(), 0);

    auto buffer = std::make_unique<std::byte[]>(1);
    stream.Read(1, buffer.get());
    ASSERT_EQ(stream.GetPosition(), 1);

    buffer = std::make_unique<std::byte[]>(3);
    stream.Read(3, buffer.get());
    ASSERT_EQ(stream.GetPosition(), 4);

    stream.Seek(zaf::SeekOrigin::Begin, 2);
    ASSERT_EQ(stream.GetPosition(), 2);

    stream.Seek(zaf::SeekOrigin::Current, 3);
    ASSERT_EQ(stream.GetPosition(), 5);

    stream.Seek(zaf::SeekOrigin::End, 0);
    ASSERT_EQ(stream.GetPosition(), 22);
}


TEST(StreamTest, GetUnderlyingBuffer) {

    std::string memory("GetUnderlyingBuffer");

    {
        auto stream = zaf::Stream::FromMemoryNoCopy(memory.data(), memory.size());
        auto buffer = stream.GetUnderlyingBuffer();
        ASSERT_EQ(buffer, reinterpret_cast<const std::byte*>(memory.data()));
    }
}


TEST(StreamTest, FromMemoryNoCopy_Read) {

    std::string memory{ "FromMemoryNotOwnFunction" };

    auto stream = zaf::Stream::FromMemoryNoCopy(memory.data(), memory.size());

    //Test get length
    ASSERT_EQ(stream.GetSize(), memory.length());

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


TEST(StreamTest, FromMemoryNoCopy_Seek) {

    std::string memory{ "FromMemoryNotOwn_Seek" };
    auto stream = zaf::Stream::FromMemoryNoCopy(memory.data(), memory.size());

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
    ASSERT_THROW(stream.Seek(zaf::SeekOrigin::Begin, -30), zaf::Error);
    ASSERT_THROW(stream.Seek(zaf::SeekOrigin::Current, -30), zaf::Error);
    ASSERT_THROW(stream.Seek(zaf::SeekOrigin::End, -30), zaf::Error);
}


TEST(StreamTest, FromMemoryNoCopy_Write) {

    std::string memory{ "FromMemoryNotOwn_Write" };
    auto stream = zaf::Stream::FromMemoryNoCopy(memory.data(), memory.size());

    ASSERT_THROW(stream.Write(L"something", 9), zaf::Error);
    ASSERT_EQ(memory, "FromMemoryNotOwn_Write");
}