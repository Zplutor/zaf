#include <gtest/gtest.h>
#include <zaf/base/error/win32_error.h>
#include <zaf/base/global_mem.h>

using namespace zaf;

static_assert(!std::is_copy_constructible_v<zaf::GlobalMem>);
static_assert(!std::is_copy_assignable_v<zaf::GlobalMem>);
static_assert(std::is_move_constructible_v<zaf::GlobalMem>);
static_assert(std::is_move_assignable_v<zaf::GlobalMem>);

static_assert(!std::is_copy_constructible_v<GlobalMemLock>);
static_assert(!std::is_copy_assignable_v<GlobalMemLock>);
static_assert(std::is_move_constructible_v<GlobalMemLock>);
static_assert(std::is_move_assignable_v<GlobalMemLock>);


TEST(GlobalMemTest, FromString) {

    auto global_mem = GlobalMem::FromString(L"abc", GlobalMemFlags::Fixed);
    ASSERT_TRUE(global_mem);
    ASSERT_GE(global_mem.Size(), 4);
    auto lock = global_mem.Lock();
    auto string = reinterpret_cast<const wchar_t*>(lock.Pointer());
    ASSERT_EQ(string[0], L'a');
    ASSERT_EQ(string[1], L'b');
    ASSERT_EQ(string[2], L'c');
    ASSERT_EQ(string[3], L'\0');
}


TEST(GlobalMemTest, Alloc) {

    //Allocate a zero size instance.
    {
        auto mem = GlobalMem::Alloc(0, GlobalMemFlags::Fixed);
        ASSERT_TRUE(mem);
        ASSERT_FALSE(mem.IsDiscarded());
        ASSERT_GT(mem.Size(), 0);
    }

    //Allocate a discared instance.
    {
        auto mem = GlobalMem::Alloc(0, GlobalMemFlags::Movable);
        ASSERT_TRUE(mem);
        ASSERT_TRUE(mem.IsDiscarded());
        ASSERT_EQ(mem.Size(), 0);
        ASSERT_THROW(mem.Lock().Pointer(), Win32Error);
    }

    {
        auto memory = zaf::GlobalMem::Alloc(10, zaf::GlobalMemFlags::Fixed);
        ASSERT_TRUE(memory.IsNull());
        ASSERT_FALSE(memory.IsDiscarded());
        ASSERT_GE(memory.Size(), 10);

        //For fixed global memory, the value of the returned pointer is equal to the value of 
        //handle.
        auto lock = memory.Lock();
        ASSERT_EQ(lock.Pointer(), memory.Handle());
    }

    {
        auto memory = zaf::GlobalMem::Alloc(10, zaf::GlobalMemFlags::Movable);
        ASSERT_TRUE(memory.IsNull());
        ASSERT_FALSE(memory.IsDiscarded());
        ASSERT_GE(memory.Size(), 10);

        //For movable global memory, the value of the returned pointer is not equal to the value of 
        //handle.
        auto lock = memory.Lock();
        ASSERT_NE(lock.Pointer(), memory.Handle());
    }
}


TEST(GlobalMemTest, Construct) {

    ASSERT_THROW(GlobalMem(nullptr), PreconditionError);
}


TEST(GlobalMemTest, Lock_LockCount) {

    auto memory = zaf::GlobalMem::Alloc(10, zaf::GlobalMemFlags::Movable);
    UINT flags{};
    {
        auto lock1 = memory.Lock();
        flags = GlobalFlags(memory.Handle());
        ASSERT_EQ(flags & GMEM_LOCKCOUNT, 1);

        {
            auto lock2 = memory.Lock();
            flags = GlobalFlags(memory.Handle());
            ASSERT_EQ(flags & GMEM_LOCKCOUNT, 2);
        }

        flags = GlobalFlags(memory.Handle());
        ASSERT_EQ(flags & GMEM_LOCKCOUNT, 1);
    }

    flags = GlobalFlags(memory.Handle());
    ASSERT_EQ(flags & GMEM_LOCKCOUNT, 0);
}
