#include <gtest/gtest.h>
#include <zaf/base/global_mem.h>

static_assert(!std::is_copy_constructible_v<zaf::GlobalMem>);
static_assert(!std::is_copy_assignable_v<zaf::GlobalMem>);
static_assert(std::is_move_constructible_v<zaf::GlobalMem>);
static_assert(std::is_move_assignable_v<zaf::GlobalMem>);

static_assert(!std::is_copy_constructible_v<zaf::GlobalMemLock>);
static_assert(!std::is_copy_assignable_v<zaf::GlobalMemLock>);
static_assert(std::is_move_constructible_v<zaf::GlobalMemLock>);
static_assert(std::is_move_assignable_v<zaf::GlobalMemLock>);


TEST(GlobalMemTest, Alloc) {

    {
        auto memory = zaf::GlobalMem::Alloc(10, zaf::GlobalMemFlags::Fixed);
        ASSERT_TRUE(memory.IsValid());

        //For fixed global memory, the value of the returned pointer is equal to the value of 
        //handle.
        auto lock = memory.Lock();
        ASSERT_EQ(lock.Pointer(), memory.Handle());
    }

    {
        auto memory = zaf::GlobalMem::Alloc(10, zaf::GlobalMemFlags::Movable);
        ASSERT_TRUE(memory.IsValid());

        //For movable global memory, the value of the returned pointer is not equal to the value of 
        //handle.
        auto lock = memory.Lock();
        ASSERT_NE(lock.Pointer(), memory.Handle());
    }
}


TEST(GlobalMemTest, Lock) {

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