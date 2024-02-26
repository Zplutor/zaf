#include <gtest/gtest.h>
#include <zaf/base/as.h>
#include <zaf/graphic/text/custom_text_inline_object.h>
#include <zaf/object/object_type.h>

using namespace zaf;

static_assert(std::is_default_constructible_v<CustomTextInlineObject>);
static_assert(!std::is_copy_constructible_v<CustomTextInlineObject>);
static_assert(!std::is_copy_assignable_v<CustomTextInlineObject>);
static_assert(!std::is_move_constructible_v<CustomTextInlineObject>);
static_assert(!std::is_move_assignable_v<CustomTextInlineObject>);


//Make sure CustomTextInlineObject can be created by its type.
TEST(CustomTextInlineObjectTest, CreateInstance) {
    ASSERT_NO_THROW(CustomTextInlineObject::Type->CreateInstance());
}