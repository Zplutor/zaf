#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/object/boxing/boxed_type_utility.h>

using namespace zaf;

static_assert(std::is_same_v<ToBoxedTypeT<bool>, Boolean>);
static_assert(std::is_same_v<ToBoxedTypeT<int>, Int32>);
static_assert(std::is_same_v<ToBoxedTypeT<std::uint32_t>, UInt32>);
static_assert(std::is_same_v<ToBoxedTypeT<std::string>, String>);
static_assert(std::is_same_v<ToBoxedTypeT<std::wstring>, WideString>);
static_assert(std::is_same_v<ToBoxedTypeT<Object>, Object>);
static_assert(std::is_same_v<ToBoxedTypeT<Control>, Control>);

static_assert(std::is_same_v<ToBoxedInstanceTypeT<bool>, std::shared_ptr<Boolean>>);
static_assert(std::is_same_v<
    ToBoxedInstanceTypeT<std::shared_ptr<bool>>, std::shared_ptr<Boolean>>);

static_assert(std::is_same_v<ToBoxedInstanceTypeT<Object>, std::shared_ptr<Object>>);
static_assert(
    std::is_same_v<ToBoxedInstanceTypeT<std::shared_ptr<Object>>, std::shared_ptr<Object>>);

static_assert(std::is_same_v<ToBoxedInstanceTypeT<Control>, std::shared_ptr<Control>>);
static_assert(
    std::is_same_v<ToBoxedInstanceTypeT<std::shared_ptr<Control>>, std::shared_ptr<Control>>);
