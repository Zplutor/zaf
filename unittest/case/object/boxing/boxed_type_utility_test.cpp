#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/object/boxing/boxed_type_utility.h>

using namespace zaf;

static_assert(std::is_same_v<ToBoxedType<bool>, Boolean>);
static_assert(std::is_same_v<ToBoxedType<int>, Int32>);
static_assert(std::is_same_v<ToBoxedType<std::uint32_t>, UInt32>);
static_assert(std::is_same_v<ToBoxedType<std::string>, String>);
static_assert(std::is_same_v<ToBoxedType<std::wstring>, WideString>);
static_assert(std::is_same_v<ToBoxedType<Object>, Object>);
static_assert(std::is_same_v<ToBoxedType<Control>, Control>);
static_assert(std::is_same_v<ToBoxedType<RECT>, BoxedObject<RECT>>);

static_assert(std::is_same_v<ToBoxedInstanceType<bool>, std::shared_ptr<Boolean>>);
static_assert(std::is_same_v<
    ToBoxedInstanceType<std::shared_ptr<bool>>, std::shared_ptr<Boolean>>);

static_assert(std::is_same_v<ToBoxedInstanceType<Object>, std::shared_ptr<Object>>);
static_assert(
    std::is_same_v<ToBoxedInstanceType<std::shared_ptr<Object>>, std::shared_ptr<Object>>);

static_assert(std::is_same_v<ToBoxedInstanceType<Control>, std::shared_ptr<Control>>);
static_assert(
    std::is_same_v<ToBoxedInstanceType<std::shared_ptr<Control>>, std::shared_ptr<Control>>);
