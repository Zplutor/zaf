#include <gtest/gtest.h>
#include <zaf/base/unique_hicon.h>

namespace {

HICON CreateTestIcon() {
    BYTE and_mask[] = { 0xFF, 0xFF };
    BYTE xor_mask[] = { 0x00, 0x00 };
    return CreateIcon(nullptr, 1, 1, 1, 1, and_mask, xor_mask);
}

bool IsIconAlive(HICON icon) {
    ICONINFO icon_info{};
    BOOL is_succeeded = GetIconInfo(icon, &icon_info);
    if (!is_succeeded) {
        return false;
    }
    if (icon_info.hbmMask) {
        DeleteObject(icon_info.hbmMask);
    }
    if (icon_info.hbmColor) {
        DeleteObject(icon_info.hbmColor);
    }
    return true;
}

bool IsIconDestroyed(HICON icon) {
    ICONINFO icon_info{};
    BOOL is_succeeded = GetIconInfo(icon, &icon_info);
    auto error = GetLastError();
    return !is_succeeded && (error == static_cast<DWORD>(ERROR_INVALID_CURSOR_HANDLE));
}

}

static_assert(!std::is_copy_constructible_v<zaf::UniqueHICON>);
static_assert(!std::is_copy_assignable_v<zaf::UniqueHICON>);
static_assert(std::is_move_constructible_v<zaf::UniqueHICON>);
static_assert(std::is_move_assignable_v<zaf::UniqueHICON>);
static_assert(std::is_nothrow_move_constructible_v<zaf::UniqueHICON>);
static_assert(std::is_nothrow_move_assignable_v<zaf::UniqueHICON>);

TEST(UniqueHICONTest, DefaultConstruct) {

    zaf::UniqueHICON icon;
    ASSERT_EQ(icon.Value(), nullptr);
}


TEST(UniqueHICONTest, ConstructWithValue) {

    HICON icon = CreateTestIcon();
    ASSERT_NE(icon, nullptr);
    {
        zaf::UniqueHICON unique_icon(icon);
        ASSERT_EQ(unique_icon.Value(), icon);
    }
    ASSERT_TRUE(IsIconDestroyed(icon));
}


TEST(UniqueHICONTest, MoveConstruct) {

    HICON icon = CreateTestIcon();
    ASSERT_NE(icon, nullptr);
    zaf::UniqueHICON unique_icon1(icon);
    zaf::UniqueHICON unique_icon2(std::move(unique_icon1));
    ASSERT_EQ(unique_icon1.Value(), nullptr);
    ASSERT_EQ(unique_icon2.Value(), icon);

    ASSERT_TRUE(IsIconAlive(icon));
}


TEST(UniqueHICONTest, MoveAssign) {

    HICON icon1 = CreateTestIcon();
    ASSERT_NE(icon1, nullptr);
    zaf::UniqueHICON unique_icon1(icon1);

    HICON icon2 = CreateTestIcon();
    ASSERT_NE(icon2, nullptr);
    zaf::UniqueHICON unique_icon2(icon2);

    unique_icon2 = std::move(unique_icon1);
    ASSERT_EQ(unique_icon1.Value(), nullptr);
    ASSERT_EQ(unique_icon2.Value(), icon1);

    ASSERT_TRUE(IsIconAlive(icon1));
    ASSERT_TRUE(IsIconDestroyed(icon2));
}


TEST(UniqueHICONTest, SelfAssign) {

    HICON icon = CreateTestIcon();
    ASSERT_NE(icon, nullptr);
    zaf::UniqueHICON unique_icon(icon);

    unique_icon = std::move(unique_icon);
    ASSERT_EQ(unique_icon.Value(), icon);
    ASSERT_TRUE(IsIconAlive(icon));
}


TEST(UniqueHICONTest, IsValid) {

    {
        zaf::UniqueHICON icon;
        ASSERT_FALSE(icon.IsValid());
        ASSERT_FALSE(bool(icon));
    }

    {
        zaf::UniqueHICON icon(nullptr);
        ASSERT_FALSE(icon.IsValid());
        ASSERT_FALSE(bool(icon));
    }

    {
        HICON icon = CreateTestIcon();
        ASSERT_NE(icon, nullptr);
        zaf::UniqueHICON unique_icon(icon);
        ASSERT_TRUE(unique_icon.IsValid());
        ASSERT_TRUE(bool(unique_icon));
    }
}


TEST(UniqueHICONTest, Reset) {

    HICON icon = CreateTestIcon();
    ASSERT_NE(icon, nullptr);
    zaf::UniqueHICON unique_icon(icon);

    unique_icon.Reset();
    ASSERT_EQ(unique_icon.Value(), nullptr);

    ASSERT_TRUE(IsIconDestroyed(icon));
}


TEST(UniqueHICONTest, ResetToNewValue) {

    {
        HICON icon = CreateTestIcon();
        ASSERT_NE(icon, nullptr);
        zaf::UniqueHICON unique_icon(icon);
        unique_icon.Reset(nullptr);
        ASSERT_EQ(unique_icon.Value(), nullptr);

        ASSERT_TRUE(IsIconDestroyed(icon));
    }

    {
        HICON icon1 = CreateTestIcon();
        ASSERT_NE(icon1, nullptr);
        zaf::UniqueHICON unique_icon(icon1);

        HICON icon2 = CreateTestIcon();
        ASSERT_NE(icon2, nullptr);
        unique_icon.Reset(icon2);
        ASSERT_EQ(unique_icon.Value(), icon2);

        ASSERT_TRUE(IsIconDestroyed(icon1));
        ASSERT_TRUE(IsIconAlive(icon2));
    }
}
