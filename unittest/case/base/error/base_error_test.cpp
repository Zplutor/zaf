#include <gtest/gtest.h>
#include <zaf/base/error/base_logic_error.h>
#include <zaf/base/error/base_runtime_error.h>
#include <zaf/base/error/base_system_error.h>

using namespace zaf;

//Base error classes are not allowed to be instantiated.
static_assert(!std::is_constructible_v<Error>);
static_assert(!std::is_constructible_v<BaseLogicError>);
static_assert(!std::is_constructible_v<BaseRuntimeError>);
static_assert(!std::is_constructible_v<BaseSystemError>);