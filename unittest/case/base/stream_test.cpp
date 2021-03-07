#include <gtest/gtest.h>
#include <zaf/base/error/error.h>
#include <zaf/base/stream.h>

TEST(StreamTest, FromMemoryNotOwn_Read) {

    std::string memory{ "FromMemoryNotOwnFunction" };

    auto stream = zaf::Stream::FromMemoryNotOwn(memory.data(), memory.size());

    //Test get length
    ASSERT_EQ(stream.GetLength(), memory.length());

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


TEST(StreamTest, FromMemoryNotOwn_Seek) {

    std::string memory{ "FromMemoryNotOwn_Seek" };
    auto stream = zaf::Stream::FromMemoryNotOwn(memory.data(), memory.size());

    auto new_seek = stream.Seek(zaf::Stream::SeekOrigin::Begin, 5);
    ASSERT_EQ(new_seek, 5);
    char buffer[10]{};
    auto read_size = stream.Read(4, buffer);
    ASSERT_EQ(read_size, 4);
    ASSERT_EQ(std::memcmp(buffer, "emor", read_size), 0);

    new_seek = stream.Seek(zaf::Stream::SeekOrigin::Current, -3);
    ASSERT_EQ(new_seek, 6);
    read_size = stream.Read(4, buffer);
    ASSERT_EQ(read_size, 4);
    ASSERT_EQ(std::memcmp(buffer, "mory", read_size), 0);

    new_seek = stream.Seek(zaf::Stream::SeekOrigin::End, -9);
    ASSERT_EQ(new_seek, 12);
    read_size = stream.Read(4, buffer);
    ASSERT_EQ(read_size, 4);
    ASSERT_EQ(std::memcmp(buffer, "tOwn", read_size), 0);

    //Seek to end
    new_seek = stream.Seek(zaf::Stream::SeekOrigin::Begin, 100);
    ASSERT_EQ(new_seek, memory.length());

    new_seek = stream.Seek(zaf::Stream::SeekOrigin::Current, 100);
    ASSERT_EQ(new_seek, memory.length());

    new_seek = stream.Seek(zaf::Stream::SeekOrigin::End, 100);
    ASSERT_EQ(new_seek, memory.length());

    //Exception
    ASSERT_THROW(stream.Seek(zaf::Stream::SeekOrigin::Begin, -30), zaf::Error);
    ASSERT_THROW(stream.Seek(zaf::Stream::SeekOrigin::Current, -30), zaf::Error);
    ASSERT_THROW(stream.Seek(zaf::Stream::SeekOrigin::End, -30), zaf::Error);
}


TEST(StreamTest, FromMemoryNotOwn_Write) {

    std::string memory{ "FromMemoryNotOwn_Write" };
    auto stream = zaf::Stream::FromMemoryNotOwn(memory.data(), memory.size());

    ASSERT_THROW(stream.Write(L"something", 9), zaf::Error);
    ASSERT_EQ(memory, "FromMemoryNotOwn_Write");
}