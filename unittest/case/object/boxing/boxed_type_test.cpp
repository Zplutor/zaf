#include <zaf/object/boxing/boxed_type.h>
#include <zaf/control/control.h>

using namespace zaf;

namespace {
class NoCustomObject { };
}

static_assert(!HasBoxedTypeV<NoCustomObject>);
static_assert(!HasBoxedTypeV<std::shared_ptr<Object>>);
static_assert(!HasBoxedTypeV<std::shared_ptr<Control>>);
static_assert(!HasBoxedTypeV<std::shared_ptr<String>>);

static_assert(HasBoxedTypeV<bool>);
static_assert(HasBoxedTypeV<bool&>);
static_assert(HasBoxedTypeV<const bool>);
static_assert(HasBoxedTypeV<int>);
static_assert(HasBoxedTypeV<int&>);
static_assert(HasBoxedTypeV<const int>);
static_assert(HasBoxedTypeV<Object>);
static_assert(HasBoxedTypeV<Object&>);
static_assert(HasBoxedTypeV<const Object>);
static_assert(HasBoxedTypeV<Control>);
static_assert(HasBoxedTypeV<Control&>);
static_assert(HasBoxedTypeV<const Control>);
static_assert(HasBoxedTypeV<String&>);
static_assert(HasBoxedTypeV<const String>);

static_assert(std::is_same_v<ToBoxedTypeT<bool>, Boolean>);
static_assert(std::is_same_v<ToBoxedTypeT<const bool>, Boolean>);
static_assert(std::is_same_v<ToBoxedTypeT<Boolean>, Boolean>);
static_assert(std::is_same_v<ToBoxedTypeT<const Boolean>, Boolean>);
static_assert(std::is_same_v<ToBoxedTypeT<int>, Int32>);
static_assert(std::is_same_v<ToBoxedTypeT<const int>, Int32>);
static_assert(std::is_same_v<ToBoxedTypeT<std::uint32_t>, UInt32>);
static_assert(std::is_same_v<ToBoxedTypeT<const std::uint32_t>, UInt32>);
static_assert(std::is_same_v<ToBoxedTypeT<std::string>, String>);
static_assert(std::is_same_v<ToBoxedTypeT<const std::string>, String>);
static_assert(std::is_same_v<ToBoxedTypeT<std::wstring>, WideString>);
static_assert(std::is_same_v<ToBoxedTypeT<const std::wstring>, WideString>);
static_assert(std::is_same_v<ToBoxedTypeT<Object>, Object>);
static_assert(std::is_same_v<ToBoxedTypeT<const Object>, Object>);
static_assert(std::is_same_v<ToBoxedTypeT<Control>, Control>);
static_assert(std::is_same_v<ToBoxedTypeT<const Control>, Control>);
static_assert(std::is_same_v<ToBoxedTypeT<String>, String>);
static_assert(std::is_same_v<ToBoxedTypeT<const String>, String>);

static_assert(std::is_same_v<ToBoxedInstanceTypeT<bool>, std::shared_ptr<Boolean>>);
static_assert(std::is_same_v<ToBoxedInstanceTypeT<bool&>, std::shared_ptr<Boolean>>);
static_assert(std::is_same_v<ToBoxedInstanceTypeT<const bool>, std::shared_ptr<Boolean>>);
static_assert(
    std::is_same_v<ToBoxedInstanceTypeT<std::shared_ptr<bool>>, std::shared_ptr<Boolean>>);
static_assert(
    std::is_same_v<ToBoxedInstanceTypeT<std::shared_ptr<bool>&>, std::shared_ptr<Boolean>>);
static_assert(
    std::is_same_v<ToBoxedInstanceTypeT<std::shared_ptr<const bool>>, std::shared_ptr<Boolean>>);

static_assert(std::is_same_v<ToBoxedInstanceTypeT<Object>, std::shared_ptr<Object>>);
static_assert(std::is_same_v<ToBoxedInstanceTypeT<Object&>, std::shared_ptr<Object>>);
static_assert(std::is_same_v<ToBoxedInstanceTypeT<const Object>, std::shared_ptr<Object>>);
static_assert(
    std::is_same_v<ToBoxedInstanceTypeT<std::shared_ptr<Object>>, std::shared_ptr<Object>>);
static_assert(
    std::is_same_v<ToBoxedInstanceTypeT<std::shared_ptr<Object>&>, std::shared_ptr<Object>>);
static_assert(
    std::is_same_v<ToBoxedInstanceTypeT<std::shared_ptr<const Object>>, std::shared_ptr<Object>>);

static_assert(std::is_same_v<ToBoxedInstanceTypeT<Control>, std::shared_ptr<Control>>);
static_assert(std::is_same_v<ToBoxedInstanceTypeT<Control&>, std::shared_ptr<Control>>);
static_assert(std::is_same_v<ToBoxedInstanceTypeT<const Control>, std::shared_ptr<Control>>);
static_assert(
    std::is_same_v<ToBoxedInstanceTypeT<std::shared_ptr<Control>>, std::shared_ptr<Control>>);
static_assert(
    std::is_same_v<ToBoxedInstanceTypeT<std::shared_ptr<Control>&>, std::shared_ptr<Control>>);
static_assert(
    std::is_same_v<ToBoxedInstanceTypeT<std::shared_ptr<const Control>>,
    std::shared_ptr<Control>>);
