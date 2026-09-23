#include <gtest/gtest.h>
#include <zaf/base/unique_handle.h>

static_assert(!std::is_copy_constructible_v<zaf::UniqueHANDLE>);
static_assert(!std::is_copy_assignable_v<zaf::UniqueHANDLE>);
static_assert(std::is_move_constructible_v<zaf::UniqueHANDLE>);
static_assert(std::is_move_assignable_v<zaf::UniqueHANDLE>);
static_assert(std::is_nothrow_move_constructible_v<zaf::UniqueHANDLE>);
static_assert(std::is_nothrow_move_assignable_v<zaf::UniqueHANDLE>);

TEST(UniqueHANDLETest, DefaultConstruct) {
    
    zaf::UniqueHANDLE handle;
    ASSERT_EQ(handle.Value(), nullptr);
}


TEST(UniqueHANDLETest, ConstructWithValue) {
    
    HANDLE handle = CreateMutex(nullptr, TRUE, nullptr);
    {
        zaf::UniqueHANDLE unique_handle(handle);
        ASSERT_EQ(unique_handle.Value(), handle);
    }
    // ReleaseMutex should fail because the handle has been closed.
    BOOL is_succeeded = ReleaseMutex(handle);
    auto error = GetLastError();
    ASSERT_FALSE(is_succeeded);
    ASSERT_EQ(error, ERROR_INVALID_HANDLE);
}


TEST(UniqueHANDLETest, MoveConstruct) {

    HANDLE handle = CreateMutex(nullptr, TRUE, nullptr);
    zaf::UniqueHANDLE unique_handle1(handle);
    zaf::UniqueHANDLE unique_handle2(std::move(unique_handle1));
    ASSERT_EQ(unique_handle1.Value(), nullptr);
    ASSERT_EQ(unique_handle2.Value(), handle);

    BOOL is_succeeded = ReleaseMutex(handle);
    ASSERT_TRUE(is_succeeded);
}


TEST(UniqueHANDLETest, MoveAssign) {

    HANDLE handle1 = CreateMutex(nullptr, TRUE, nullptr);
    zaf::UniqueHANDLE unique_handle1(handle1);

    HANDLE handle2 = CreateMutex(nullptr, TRUE, nullptr);
    zaf::UniqueHANDLE unique_handle2(handle2);

    unique_handle2 = std::move(unique_handle1);
    ASSERT_EQ(unique_handle1.Value(), nullptr);
    ASSERT_EQ(unique_handle2.Value(), handle1);

    BOOL is_succeeded = ReleaseMutex(handle1);
    ASSERT_TRUE(is_succeeded);

    is_succeeded = ReleaseMutex(handle2);
    auto error = GetLastError();
    ASSERT_FALSE(is_succeeded);
    ASSERT_EQ(error, ERROR_INVALID_HANDLE);
}


TEST(UniqueHANDLETest, SelfAssign) {

    HANDLE handle = CreateMutex(nullptr, TRUE, nullptr);
    zaf::UniqueHANDLE unique_handle(handle);

    unique_handle = std::move(unique_handle);
    BOOL is_succeeded = ReleaseMutex(handle);
    ASSERT_TRUE(is_succeeded);
}


TEST(UniqueHANDLETest, IsValid) {

    {
        zaf::UniqueHANDLE handle;
        ASSERT_FALSE(handle.IsValid());
        ASSERT_FALSE(bool(handle));
    }

    {
        zaf::UniqueHANDLE handle(nullptr);
        ASSERT_FALSE(handle.IsValid());
        ASSERT_FALSE(bool(handle));
    }

    {
        zaf::UniqueHANDLE handle(INVALID_HANDLE_VALUE);
        ASSERT_FALSE(handle.IsValid());
        ASSERT_FALSE(bool(handle));
    }

    {
        HANDLE handle = CreateMutex(nullptr, TRUE, nullptr);
        zaf::UniqueHANDLE unique_handle(handle);
        ASSERT_TRUE(unique_handle.IsValid());
        ASSERT_TRUE(bool(unique_handle));
    }
}


TEST(UniqueHANDLETest, Reset) {

    HANDLE handle = CreateMutex(nullptr, TRUE, nullptr);
    zaf::UniqueHANDLE unique_handle(handle);

    HANDLE* pointer = unique_handle.Reset();
    ASSERT_NE(pointer, nullptr);
    ASSERT_EQ(*pointer, nullptr);
    ASSERT_EQ(unique_handle.Value(), nullptr);

    BOOL is_succeeded = ReleaseMutex(handle);
    auto error = GetLastError();
    ASSERT_FALSE(is_succeeded);
    ASSERT_EQ(error, ERROR_INVALID_HANDLE);
}


TEST(UniqueHANDLETest, ResetToNewValue) {

    {
        HANDLE handle = CreateMutex(nullptr, TRUE, nullptr);
        zaf::UniqueHANDLE unique_handle(handle);
        unique_handle.Reset(nullptr);
        ASSERT_EQ(unique_handle.Value(), nullptr);

        BOOL is_succeeded = ReleaseMutex(handle);
        auto error = GetLastError();
        ASSERT_FALSE(is_succeeded);
        ASSERT_EQ(error, ERROR_INVALID_HANDLE);
    }

    {
        HANDLE handle1 = CreateMutex(nullptr, TRUE, nullptr);
        zaf::UniqueHANDLE unique_handle(handle1);

        HANDLE handle2 = CreateMutex(nullptr, TRUE, nullptr);
        unique_handle.Reset(handle2);
        ASSERT_EQ(unique_handle.Value(), handle2);

        BOOL is_succeeded = ReleaseMutex(handle1);
        auto error = GetLastError();
        ASSERT_FALSE(is_succeeded);
        ASSERT_EQ(error, ERROR_INVALID_HANDLE);
    }
}