#include <optional>
#include <gtest/gtest.h>
#include <zaf/base/error/win32_error.h>

using namespace zaf;

TEST(Win32ErrorTest, Category) {

    ASSERT_STREQ(Win32Error::Category().name(), "Win32");
}


TEST(Win32ErrorTest, ThrowIf) {

    ASSERT_NO_THROW(ZAF_THROW_IF_WIN32_ERROR(0));

    std::optional<Win32Error> catched_error;
    int prior_line{};

    try {
        prior_line = __LINE__;
        ZAF_THROW_IF_WIN32_ERROR(5);
    }
    catch (const Win32Error& error) {
        catched_error = error;
    }

    ASSERT_TRUE(catched_error.has_value());
    ASSERT_EQ(catched_error->Code().category(), Win32Error::Category());
    ASSERT_EQ(catched_error->Code().value(), 5);

    ASSERT_STREQ(catched_error->Site().GetFile(), __FILE__);
    ASSERT_EQ(catched_error->Site().GetLine(), prior_line + 1);
}

