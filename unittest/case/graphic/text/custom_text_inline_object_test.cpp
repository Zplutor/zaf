#include <gtest/gtest.h>
#include <zaf/base/as.h>
#include <zaf/control/textual/inline_object.h>
#include <zaf/object/object_type.h>

using namespace zaf::textual;

static_assert(std::is_default_constructible_v<InlineObject>);
static_assert(!std::is_copy_constructible_v<InlineObject>);
static_assert(!std::is_copy_assignable_v<InlineObject>);
static_assert(!std::is_move_constructible_v<InlineObject>);
static_assert(!std::is_move_assignable_v<InlineObject>);


//Make sure InlineObject can be created by its type.
TEST(InlineObjectTest, CreateInstance) {
    ASSERT_NO_THROW(InlineObject::Type->CreateInstance());
}