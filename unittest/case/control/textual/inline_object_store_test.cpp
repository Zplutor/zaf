#include <gtest/gtest.h>
#include <zaf/control/textual/inline_object.h>
#include <zaf/control/textual/inline_object_store.h>
#include <zaf/creation.h>

using namespace zaf;
using namespace zaf::textual;

static_assert(!std::is_copy_constructible_v<InlineObjectStore>);
static_assert(!std::is_copy_assignable_v<InlineObjectStore>);
static_assert(std::is_move_constructible_v<InlineObjectStore>);
static_assert(std::is_move_assignable_v<InlineObjectStore>);


TEST(InlineObjectStoreTest, AttachObjectMultipleTimes) {

    auto inline_object = Create<InlineObject>();

    InlineObjectStore collection1;
    collection1.Attach(inline_object, Range{ 0, 10 });

    InlineObjectStore collection2;
    ASSERT_THROW(collection2.Attach(inline_object, Range{ 0 , 1 }), std::logic_error);
}


TEST(InlineObjectStoreTest, AttachObjectAfterDestruction) {

    auto inline_object = Create<InlineObject>();
    {
        InlineObjectStore collection1;
        collection1.Attach(inline_object, Range{ 0, 10 });
    }

    InlineObjectStore collection2;
    //The object should be able to be attached to another collection again.
    ASSERT_NO_THROW(collection2.Attach(inline_object, Range{ 0 , 1 }));
}