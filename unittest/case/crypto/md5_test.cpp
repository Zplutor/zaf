#include <gtest/gtest.h>
#include <Windows.h>
#include <bcrypt.h>
#include <zaf/crypto/md5.h>

using namespace zaf::crypto;

static_assert(!std::is_copy_constructible_v<MD5>);
static_assert(!std::is_copy_assignable_v<MD5>);
static_assert(std::is_move_constructible_v<MD5>);
static_assert(std::is_move_assignable_v<MD5>);

TEST(MD5Test, Move) {

    {
        MD5 md5;
        auto handle = md5.Handle();

        MD5 md5_moved{ std::move(md5) };
        ASSERT_EQ(md5.Handle(), nullptr);
        ASSERT_EQ(md5_moved.Handle(), handle);
    }

    {
        MD5 md5;
        auto handle = md5.Handle();

        MD5 md5_moved;
        auto handle_moved = md5_moved.Handle();

        md5_moved = std::move(md5);
        ASSERT_EQ(md5.Handle(), nullptr);
        ASSERT_EQ(md5_moved.Handle(), handle);

        NTSTATUS status = BCryptDestroyHash(handle_moved);
        ASSERT_NE(status, 0);
    }
}


TEST(MD5Test, MD5) {

    auto test = [](std::string_view input, std::wstring_view expected) {
        MD5 md5;
        md5.Update(input.data(), input.length());
        return md5.Finish().ToHexString() == expected;
    };

    ASSERT_TRUE(test("", L"D41D8CD98F00B204E9800998ECF8427E"));
    ASSERT_TRUE(test("abcdefg", L"7AC66C0F148DE9519B8BD264312C4D64"));
}