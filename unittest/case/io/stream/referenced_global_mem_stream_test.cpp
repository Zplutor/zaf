#include <gtest/gtest.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/base/global_mem.h>
#include <zaf/io/stream/stream.h>

using namespace zaf;

TEST(ReferencedGlobalMemStreamTest, CreateOnMutableGlobalMem) {

    {
        GlobalMem mem;
        ASSERT_THROW(Stream::CreateOnGlobalMem(GlobalMem{}), PreconditionError);
    }

    {
        auto mem = GlobalMem::Alloc(10, GlobalMemFlags::Fixed);
        auto stream = Stream::CreateOnGlobalMem(mem);
        ASSERT_TRUE(stream);
        ASSERT_TRUE(stream.CanWrite());
        stream.Write("112", 3);

        auto mem_lock = mem.Lock();
        auto pointer = reinterpret_cast<const char*>(mem_lock.Pointer());
        ASSERT_EQ(pointer[0], '1');
        ASSERT_EQ(pointer[1], '1');
        ASSERT_EQ(pointer[2], '2');
    }
}


TEST(ReferencedGlobalMemStreamTest, CreateOnConstGlobalMem) {

    {
        const GlobalMem mem;
        ASSERT_THROW(Stream::CreateOnGlobalMem(GlobalMem{}), PreconditionError);
    }

    {
        const auto mem = GlobalMem::FromString(L"tees", GlobalMemFlags::Fixed);
        auto stream = Stream::CreateOnGlobalMem(mem);
        ASSERT_TRUE(stream);
        ASSERT_FALSE(stream.CanWrite());

        wchar_t buffer[4]{};
        auto read_size = stream.Read(4 * sizeof(wchar_t), buffer);
        ASSERT_EQ(read_size, 4 * sizeof(wchar_t));

        ASSERT_EQ(buffer[0], L't');
        ASSERT_EQ(buffer[1], L'e');
        ASSERT_EQ(buffer[2], L'e');
        ASSERT_EQ(buffer[3], L's');
    }
}