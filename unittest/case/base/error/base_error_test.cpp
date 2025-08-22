#include <gtest/gtest.h>
#include <zaf/base/error/logic_error_base.h>
#include <zaf/base/error/runtime_error_base.h>
#include <zaf/base/error/system_error_base.h>

using namespace zaf;

//Base error classes are not allowed to be instantiated.
static_assert(!std::is_constructible_v<Error>);
static_assert(!std::is_constructible_v<LogicErrorBase>);
static_assert(!std::is_constructible_v<RuntimeErrorBase>);
static_assert(!std::is_constructible_v<SystemErrorBase>);