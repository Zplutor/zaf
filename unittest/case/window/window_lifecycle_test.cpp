#include <gtest/gtest.h>
#include <zaf/base/error/win32_error.h>
#include <zaf/window/invalid_handle_state_error.h>
#include <zaf/window/window.h>
#include "window_test.h"

namespace zaf::testing {

TEST_F(WindowTest, Close_NotCreated) {
    auto window = zaf::Create<zaf::Window>();
    bool is_event_raised = false;
    auto sub = window->ClosingEvent().Subscribe([&](const zaf::ClosingInfo&) {
        is_event_raised = true;
    });
    window->Close();
    ASSERT_EQ(window->HandleState(), zaf::WindowHandleState::Destroyed);
    ASSERT_FALSE(is_event_raised);
}


TEST_F(WindowTest, Close_Creating) {

    // Close normally.
    {
        auto window = zaf::Create<zaf::Window>();
        auto sub = window->HandleCreatingEvent().Subscribe([&](const zaf::HandleCreatingInfo&) {
            window->Close();
            ASSERT_EQ(window->HandleState(), zaf::WindowHandleState::Destroyed);
        });
        ASSERT_THROW(auto holder = window->CreateHandle(), zaf::Win32Error);
    }

    // Prevent closing with ClosingEvent
    {
        auto window = zaf::Create<zaf::Window>();
        auto sub1 = window->ClosingEvent().Subscribe([&](const zaf::ClosingInfo& info) {
            info.SetCanClose(false);
        });
        auto sub2 = window->HandleCreatingEvent().Subscribe([&](const zaf::HandleCreatingInfo&) {
            window->Close();
            ASSERT_EQ(window->HandleState(), zaf::WindowHandleState::Creating);
        });
        auto holder = window->CreateHandle();
        ASSERT_EQ(window->HandleState(), zaf::WindowHandleState::Created);
    }
}


TEST_F(WindowTest, Close_Created) {

    // Close normally.
    {
        auto window = zaf::Create<zaf::Window>();
        auto holder = window->CreateHandle();
        window->Close();
        ASSERT_EQ(window->HandleState(), zaf::WindowHandleState::Destroyed);
    }

    // Prevent closing with ClosingEvent
    {
        auto window = zaf::Create<zaf::Window>();
        auto holder = window->CreateHandle();
        auto sub = window->ClosingEvent().Subscribe([&](const zaf::ClosingInfo& info) {
            info.SetCanClose(false);
        });
        window->Close();
        ASSERT_EQ(window->HandleState(), zaf::WindowHandleState::Created);
    }
}


TEST_F(WindowTest, Close_Destroying) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();

    bool is_event_raised{};
    auto sub1 = window->ClosingEvent().Subscribe([&](const zaf::ClosingInfo&) {
        is_event_raised = true;
    });

    bool has_asserted{};
    auto sub = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        ASSERT_NO_THROW(window->Close());
        has_asserted = true;
    });
    window->Destroy();
    ASSERT_TRUE(has_asserted);
    ASSERT_FALSE(is_event_raised);
}


TEST_F(WindowTest, Close_Destroyed) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();

    bool is_event_raised{};
    auto sub1 = window->ClosingEvent().Subscribe([&](const zaf::ClosingInfo&) {
        is_event_raised = true;
    });

    window->Destroy();
    ASSERT_NO_THROW(window->Close());
    ASSERT_FALSE(is_event_raised);
}


TEST_F(WindowTest, DestroyDuringHandlingMessage) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    bool has_asserted{};
    auto sub = window->MessageHandlingEvent().Subscribe([&](
        const zaf::MessageHandlingInfo& event_info) {

        if (event_info.Message().ID() == WM_LBUTTONDOWN) {
            window->Destroy();
        }
    });

    window->Messager().SendWMLBUTTONDOWN({});
}

}