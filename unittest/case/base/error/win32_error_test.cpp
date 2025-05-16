#include <optional>
#include <gtest/gtest.h>
#include <zaf/base/error/win32_error.h>

using namespace zaf;

TEST(Win32ErrorTest, Category) {

    ASSERT_STREQ(Win32Error::Category().name(), "zaf.Win32");
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
    ASSERT_EQ(catched_error->code().category(), Win32Error::Category());
    ASSERT_EQ(catched_error->code().value(), 5);

    ASSERT_EQ(catched_error->Location().File(), __FILE__);
    ASSERT_EQ(catched_error->Location().Line(), prior_line + 1);
}

