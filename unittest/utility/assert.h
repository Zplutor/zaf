#pragma once

#include <gtest/gtest.h>

#define ASSERT_THROW_ERRC(expression, errc)  \
try {                                        \
    expression;                              \
    GTEST_FAIL();                            \
}                                            \
catch (const zaf::Error& error) {            \
    ASSERT_TRUE(error.Code() == errc);       \
}                                            