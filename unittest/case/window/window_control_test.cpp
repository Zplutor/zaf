#include <gtest/gtest.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/control/control.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include "window_test.h"

namespace zaf::testing {

TEST_F(WindowTest, RootControl) {

    auto window = zaf::Create<zaf::Window>();
    auto root_control = window->RootControl();
    ASSERT_NE(root_control, nullptr);
    ASSERT_EQ(root_control->Window(), window);
    ASSERT_EQ(root_control->Parent(), nullptr);
}


TEST_F(WindowTest, SetRootControl_InvalidArgument) {

    auto window = zaf::Create<zaf::Window>();
    auto old_root_control = window->RootControl();

    // Setting null.
    ASSERT_THROW(window->SetRootControl(nullptr), zaf::PreconditionError);
    ASSERT_EQ(window->RootControl(), old_root_control);

    // Setting a control which already has a parent.
    {
        auto parent = zaf::Create<zaf::Control>();
        auto child = zaf::Create<zaf::Control>();
        parent->AddChild(child);
        ASSERT_THROW(window->SetRootControl(child), zaf::InvalidOperationError);
        ASSERT_EQ(window->RootControl(), old_root_control);
    }

    // Setting a control which is already a root control of another window.
    {
        auto other_window = zaf::Create<zaf::Window>();
        ASSERT_THROW(
            window->SetRootControl(other_window->RootControl()), zaf::InvalidOperationError);
        ASSERT_EQ(window->RootControl(), old_root_control);
    }
}


TEST_F(WindowTest, SetRootControl_SameControl) {

    auto window = zaf::Create<zaf::Window>();

    bool is_event_called{};
    auto sub = window->RootControlChangedEvent().Subscribe(
        [&](const zaf::RootControlChangedInfo& event_info) {
            is_event_called = true;
        });

    auto old_root_control = window->RootControl();
    // Setting the same root control multiple times is allowed.
    ASSERT_NO_THROW(window->SetRootControl(old_root_control));
    ASSERT_EQ(window->RootControl(), old_root_control);
    ASSERT_FALSE(is_event_called);
}


TEST_F(WindowTest, SetRootControl_NewControl) {

    auto window = zaf::Create<zaf::Window>();

    bool is_event_called{};
    auto sub = window->RootControlChangedEvent().Subscribe(
        [&](const zaf::RootControlChangedInfo& event_info) {
            is_event_called = true;
        });

    auto old_root_control = window->RootControl();
    auto new_root_control = zaf::Create<zaf::Control>();

    window->SetRootControl(new_root_control);
    ASSERT_EQ(window->RootControl(), new_root_control);
    ASSERT_EQ(new_root_control->Window(), window);
    ASSERT_EQ(new_root_control->Parent(), nullptr);
    ASSERT_EQ(old_root_control->Window(), nullptr);
    ASSERT_TRUE(is_event_called);
}

}