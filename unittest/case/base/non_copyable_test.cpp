#include <gtest/gtest.h>
#include <zaf/base/non_copyable.h>

class NonCopyable_Movable : zaf::NonCopyable { };
static_assert(!std::is_copy_constructible_v<NonCopyable_Movable>);
static_assert(!std::is_copy_assignable_v<NonCopyable_Movable>);
static_assert(std::is_move_constructible_v<NonCopyable_Movable>);
static_assert(std::is_move_assignable_v<NonCopyable_Movable>);


class NonCopyable_NonMovable : zaf::NonCopyableNonMovable {};
static_assert(!std::is_copy_constructible_v<NonCopyable_NonMovable>);
static_assert(!std::is_copy_assignable_v<NonCopyable_NonMovable>);
static_assert(!std::is_move_constructible_v<NonCopyable_NonMovable>);
static_assert(!std::is_move_assignable_v<NonCopyable_NonMovable>);