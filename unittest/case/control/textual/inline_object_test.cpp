#include <gtest/gtest.h>
#include <zaf/base/as.h>
#include <zaf/control/textual_control.h>
#include <zaf/control/textual/inline_object.h>
#include <zaf/creation.h>
#include <zaf/object/object_type.h>

using namespace zaf;
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


TEST(InlineObjectTest, Host) {

    auto object = Create<InlineObject>();
    bool attached_event_called{};
    bool detached_event_called{};
    auto attached_sub = object->AttachedEvent().Subscribe([&](const AttachedInfo&) {
        attached_event_called = true;
    });
    auto detached_sub = object->DetachedEvent().Subscribe([&](const DetachedInfo&) {
        detached_event_called = true;
    });

    //Host is nullptr at first.
    ASSERT_EQ(object->Host(), nullptr);

    auto control = Create<TextualControl>();
    control->SetText(L"object");

    //Host is equal to the textual control after attaching.
    control->AttachInlineObjectToRange(object, Range{ 0, 6 });
    ASSERT_EQ(object->Host(), control);
    ASSERT_TRUE(attached_event_called);

    //Host is nullptr after detaching.
    control->SetText(L"new object");
    ASSERT_EQ(object->Host(), nullptr);
    ASSERT_TRUE(detached_event_called);

    //Attach again.
    attached_event_called = false;
    detached_event_called = false;
    control->AttachInlineObjectToRange(object, Range{ 4, 6 });
    ASSERT_EQ(object->Host(), control);
    ASSERT_TRUE(attached_event_called);

    //Host is nullptr if the textual control is destoryed.
    control.reset();
    ASSERT_EQ(object->Host(), nullptr);
    ASSERT_TRUE(detached_event_called);
}