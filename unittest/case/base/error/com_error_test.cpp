#include <optional>
#include <gtest/gtest.h>
#include <zaf/base/error/com_error.h>

using namespace zaf;

TEST(COMErrorTest, Category) {

    ASSERT_STREQ(COMError::Category().name(), "zaf.COM");
}


TEST(COMErrorTest, ThrowIf) {

    ASSERT_NO_THROW(ZAF_THROW_IF_COM_ERROR(0));

    std::optional<COMError> catched_error;
    int prior_line{};

    try {
        prior_line = __LINE__;
        ZAF_THROW_IF_COM_ERROR(static_cast<HRESULT>(0x80000005));
    }
    catch (const COMError& error) {
        catched_error = error;
    }

    ASSERT_TRUE(catched_error.has_value());
    ASSERT_EQ(catched_error->code().category(), COMError::Category());
    ASSERT_EQ(catched_error->code().value(), 0x80000005);

    ASSERT_STREQ(catched_error->Location().File(), __FILE__);
    ASSERT_EQ(catched_error->Location().Line(), prior_line + 1);
}

