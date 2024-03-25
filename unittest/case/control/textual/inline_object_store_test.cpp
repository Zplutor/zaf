#include <gtest/gtest.h>
#include <zaf/control/textual/inline_object.h>
#include <zaf/control/textual/inline_object_store.h>
#include <zaf/creation.h>

using namespace zaf::textual;

namespace zaf::test {

//Use this test fixture to get access to the private members of InlineObject.
class InlineObjectStoreTest : public testing::Test {
protected:
    std::optional<Range> GetAttachedRange(const InlineObject& object) {
        return object.attached_range_;
    }
};


static_assert(!std::is_copy_constructible_v<InlineObjectStore>);
static_assert(!std::is_copy_assignable_v<InlineObjectStore>);
static_assert(std::is_move_constructible_v<InlineObjectStore>);
static_assert(std::is_move_assignable_v<InlineObjectStore>);


TEST_F(InlineObjectStoreTest, AttachObjectMultipleTimes) {

    auto inline_object = Create<InlineObject>();

    InlineObjectStore collection1;
    collection1.Attach(inline_object, Range{ 0, 10 });

    InlineObjectStore collection2;
    ASSERT_THROW(collection2.Attach(inline_object, Range{ 0 , 1 }), std::logic_error);
}


TEST_F(InlineObjectStoreTest, AttachObjectAfterDestruction) {

    auto inline_object = Create<InlineObject>();
    {
        InlineObjectStore collection1;
        collection1.Attach(inline_object, Range{ 0, 10 });
    }

    //The attached range should be cleared after destruction.
    ASSERT_EQ(GetAttachedRange(*inline_object), std::nullopt);
}


TEST_F(InlineObjectStoreTest, Clone) {

    InlineObjectStore store;
    auto cloned = store.Clone();
    ASSERT_TRUE(cloned.Items().empty());

    auto object1 = Create<InlineObject>();
    auto object2 = Create<InlineObject>();
    store.Attach(object1, Range{ 1, 5 });
    store.Attach(object2, Range{ 10, 2 });

    cloned = store.Clone();
    ASSERT_EQ(cloned.Items().size(), 2);

    auto cloned_object1 = cloned.Items()[0].Object();
    ASSERT_NE(cloned_object1, object1);
    ASSERT_EQ(GetAttachedRange(*cloned_object1), GetAttachedRange(*object1));

    auto cloned_object2 = cloned.Items()[1].Object();
    ASSERT_NE(cloned_object2, object2);
    ASSERT_EQ(GetAttachedRange(*cloned_object2), GetAttachedRange(*object2));
}

}