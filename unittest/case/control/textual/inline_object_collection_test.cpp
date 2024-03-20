#include <gtest/gtest.h>
#include <zaf/control/textual/inline_object.h>
#include <zaf/control/textual/inline_object_collection.h>
#include <zaf/creation.h>

using namespace zaf;
using namespace zaf::textual;

static_assert(!std::is_copy_constructible_v<InlineObjectCollection>);
static_assert(!std::is_copy_assignable_v<InlineObjectCollection>);
static_assert(std::is_move_constructible_v<InlineObjectCollection>);
static_assert(std::is_move_assignable_v<InlineObjectCollection>);


TEST(InlineObjectCollectionTest, AttachObjectMultipleTimes) {

    auto inline_object = Create<InlineObject>();

    InlineObjectCollection collection1;
    collection1.Attach(inline_object, Range{ 0, 10 });

    InlineObjectCollection collection2;
    ASSERT_THROW(collection2.Attach(inline_object, Range{ 0 , 1 }), std::logic_error);
}


TEST(InlineObjectCollectionTest, AttachObjectAfterDestruction) {

    auto inline_object = Create<InlineObject>();
    {
        InlineObjectCollection collection1;
        collection1.Attach(inline_object, Range{ 0, 10 });
    }

    InlineObjectCollection collection2;
    //The object should be able to be attached to another collection again.
    ASSERT_NO_THROW(collection2.Attach(inline_object, Range{ 0 , 1 }));
}