#include <gtest/gtest.h>
#include <zaf/base/byte_array.h>
#include <zaf/base/range.h>

TEST(ByteArrayTest, FromMemory) {

    ASSERT_THROW(zaf::ByteArray::FromMemory(nullptr, 0), zaf::PreconditionError);

    std::uint8_t memory[]{ '\x10', '\x11', '\x12' };
    auto byte_array = zaf::ByteArray::FromMemory(memory, 0);
    ASSERT_TRUE(byte_array.IsEmpty());

    byte_array = zaf::ByteArray::FromMemory(memory, 2);
    ASSERT_EQ(byte_array.Size(), 2);
    ASSERT_EQ(byte_array[0], std::byte('\x10'));
    ASSERT_EQ(byte_array[1], std::byte('\x11'));
}


TEST(ByteArrayTest, FromString) {

    auto byte_array = zaf::ByteArray::FromString("narrow");
    ASSERT_EQ(byte_array.Size(), 6);
    ASSERT_EQ(byte_array[0], std::byte('n'));
    ASSERT_EQ(byte_array[1], std::byte('a'));
    ASSERT_EQ(byte_array[2], std::byte('r'));
    ASSERT_EQ(byte_array[3], std::byte('r'));
    ASSERT_EQ(byte_array[4], std::byte('o'));
    ASSERT_EQ(byte_array[5], std::byte('w'));
}


TEST(ByteArrayTest, FromWideString) {

    auto byte_array = zaf::ByteArray::FromString(L"wide");
    ASSERT_EQ(byte_array.Size(), 8);
    ASSERT_EQ(byte_array[0], std::byte('w'));
    ASSERT_EQ(byte_array[1], std::byte());
    ASSERT_EQ(byte_array[2], std::byte('i'));
    ASSERT_EQ(byte_array[3], std::byte());
    ASSERT_EQ(byte_array[4], std::byte('d'));
    ASSERT_EQ(byte_array[5], std::byte());
    ASSERT_EQ(byte_array[6], std::byte('e'));
    ASSERT_EQ(byte_array[7], std::byte());
}


TEST(ByteArrayTest, Construction) {

    {
        zaf::ByteArray byte_array;
        ASSERT_EQ(byte_array.Size(), 0);
    }

    {
        zaf::ByteArray byte_array{ 38 };
        ASSERT_EQ(byte_array.Size(), 38);
        for (auto index : zaf::Range(0, byte_array.Size())) {
            ASSERT_EQ(byte_array[index], std::byte(0));
        }
    }

    //Initialier list
    {
        zaf::ByteArray byte_array{ std::byte(1), std::byte(2), std::byte(3) };
        ASSERT_EQ(byte_array.Size(), 3);
        ASSERT_EQ(byte_array[0], std::byte(1));
        ASSERT_EQ(byte_array[1], std::byte(2));
        ASSERT_EQ(byte_array[2], std::byte(3));
    }
}


TEST(ByteArrayTest, Append) {

    zaf::ByteArray byte_array;
    byte_array.Append(std::byte(1));
    byte_array.Append(std::byte(2));

    ASSERT_EQ(byte_array.Size(), 2);
    ASSERT_EQ(byte_array[0], std::byte(1));
    ASSERT_EQ(byte_array[1], std::byte(2));
}


TEST(ByteArrayTest, BeginEnd) {

    zaf::ByteArray byte_array(4);
    byte_array[0] = std::byte(3);
    byte_array[1] = std::byte(4);
    byte_array[2] = std::byte(5);
    byte_array[3] = std::byte(6);

    {
        std::size_t index{};
        for (auto iterator = byte_array.begin(); iterator != byte_array.end(); ++iterator) {
            ASSERT_EQ(*iterator, std::byte(index++ + 3));
        }
    }

    {
        const auto& const_array = byte_array;
        std::size_t index{};
        for (auto iterator = const_array.begin(); iterator != const_array.end(); ++iterator) {
            ASSERT_EQ(*iterator, std::byte(index++ + 3));
        }
    }

    {
        std::size_t index{};
        for (auto iterator = byte_array.cbegin(); iterator != byte_array.cend(); ++iterator) {
            ASSERT_EQ(*iterator, std::byte(index++ + 3));
        }
    }
}


TEST(ByteArrayTest, RangeFor) {

    zaf::ByteArray byte_array(4);
    std::size_t index = 10;
    for (auto& each_byte : byte_array) {
        each_byte = std::byte(index++);
    }
    ASSERT_EQ(byte_array[0], std::byte(10));
    ASSERT_EQ(byte_array[1], std::byte(11));
    ASSERT_EQ(byte_array[2], std::byte(12));
    ASSERT_EQ(byte_array[3], std::byte(13));

    index = 10;
    for (auto each_byte : byte_array) {
        ASSERT_EQ(each_byte, std::byte(index++));
    }
}


TEST(ByteArrayTest, Compare) {

    {
        zaf::ByteArray array1;
        zaf::ByteArray array2;
        ASSERT_TRUE(array1 == array2);
        ASSERT_FALSE(array1 != array2);
    }

    {
        zaf::ByteArray array1(3);
        array1[0] = std::byte(100);
        array1[1] = std::byte(98);
        array1[2] = std::byte(96);
        zaf::ByteArray array2(3);
        array2[0] = std::byte(100);
        array2[1] = std::byte(98);
        array2[2] = std::byte(96);
        ASSERT_TRUE(array1 == array2);
        ASSERT_FALSE(array1 != array2);
    }

    {
        zaf::ByteArray array1(2);
        array1[0] = std::byte(50);
        array1[1] = std::byte(78);
        zaf::ByteArray array2(2);
        array2[0] = std::byte(100);
        array2[0] = std::byte(200);
        ASSERT_FALSE(array1 == array2);
        ASSERT_TRUE(array1 != array2);
        ASSERT_TRUE(array1 < array2);
        ASSERT_FALSE(array1 > array2);
    }
}


TEST(ByteArrayTest, ToHexString) {

    zaf::ByteArray byte_array(5);
    byte_array[0] = std::byte(0xff);
    byte_array[1] = std::byte(0x00);
    byte_array[2] = std::byte(0x10);
    byte_array[3] = std::byte(0xca);
    byte_array[4] = std::byte(0x8a);
    ASSERT_EQ(byte_array.ToHexString(), L"FF0010CA8A");
}