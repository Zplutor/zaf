#include <gtest/gtest.h>
#include <zaf/window/invalid_handle_state_error.h>
#include <zaf/window/window.h>
#include "window_test.h"

namespace zaf::testing {

TEST_F(WindowTest, ActivateOptions_DefaultValue) {
    auto window = zaf::Create<zaf::Window>();
    ASSERT_EQ(window->ActivateOptions(), zaf::ActivateOptions::Normal);
}


TEST_F(WindowTest, ActivateOptions_InVariantStates) {

    auto window = zaf::Create<zaf::Window>();
    window->SetActivateOptions(zaf::ActivateOptions::NoActivate);

    // NotCreated state
    ASSERT_EQ(window->ActivateOptions(), zaf::ActivateOptions::NoActivate);

    // Creating state
    zaf::ActivateOptions options_in_creating{};
    auto sub = window->HandleCreatingEvent().Subscribe([&](const zaf::HandleCreatingInfo&) {
        options_in_creating = window->ActivateOptions();
    });
    auto holder = window->CreateHandle();
    ASSERT_EQ(options_in_creating, zaf::ActivateOptions::NoActivate);

    // Created state
    ASSERT_EQ(window->ActivateOptions(), zaf::ActivateOptions::NoActivate);

    // Destroying state
    zaf::ActivateOptions options_in_destroying{};
    auto sub2 = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        options_in_destroying = window->ActivateOptions();
    });
    window->Destroy();
    ASSERT_EQ(options_in_destroying, zaf::ActivateOptions::NoActivate);

    // Destroyed state
    ASSERT_EQ(window->ActivateOptions(), zaf::ActivateOptions::NoActivate);
}


TEST_F(WindowTest, SetActivateOptions_InVariantStates) {

    // NotCreated state
    {
        auto window = zaf::Create<zaf::Window>();
        window->SetActivateOptions(zaf::ActivateOptions::NoActivate);
        ASSERT_EQ(window->ActivateOptions(), zaf::ActivateOptions::NoActivate);
    }

    // Creating state
    {
        auto window = zaf::Create<zaf::Window>();
        window->SetActivateOptions(zaf::ActivateOptions::Normal);
        bool has_asserted{};
        auto sub = window->HandleCreatingEvent().Subscribe([&](const zaf::HandleCreatingInfo&) {
            has_asserted = true;
            window->SetActivateOptions(zaf::ActivateOptions::NoActivate);
            ASSERT_EQ(window->ActivateOptions(), zaf::ActivateOptions::NoActivate);
        });
        auto holder = window->CreateHandle();
        ASSERT_TRUE(has_asserted);
        window->Destroy();
    }

    // Created state
    {
        auto window = zaf::Create<zaf::Window>();
        window->SetActivateOptions(zaf::ActivateOptions::Normal);

        auto holder = window->CreateHandle();
        window->SetActivateOptions(zaf::ActivateOptions::NoActivate);
        ASSERT_EQ(window->ActivateOptions(), zaf::ActivateOptions::NoActivate);
    }

    // Destroying/Destroyed state
    {
        auto window = zaf::Create<zaf::Window>();
        window->SetActivateOptions(zaf::ActivateOptions::Normal);
        auto holder = window->CreateHandle();
        bool has_asserted{};
        auto sub = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
            has_asserted = true;
            window->SetActivateOptions(zaf::ActivateOptions::NoActivate);
            ASSERT_EQ(window->ActivateOptions(), zaf::ActivateOptions::NoActivate);
        });
        window->Destroy();
        ASSERT_TRUE(has_asserted);

        ASSERT_THROW(
            window->SetActivateOptions(zaf::ActivateOptions::NoActivate),
            zaf::InvalidHandleStateError);
    }
}


TEST_F(WindowTest, ActivateOptions_CheckStyle) {

    auto test = [](zaf::ActivateOptions options, bool expected_has_style) {
        auto window = zaf::Create<zaf::Window>();
        window->SetActivateOptions(options);
        auto holder = window->CreateHandle();
        LONG ex_style = GetWindowLong(window->Handle(), GWL_EXSTYLE);
        bool has_style = (ex_style & WS_EX_NOACTIVATE) != 0;
        ASSERT_EQ(has_style, expected_has_style);
    };
    test(zaf::ActivateOptions::Normal, false);
    test(zaf::ActivateOptions::NoAutoActivate, true);
    test(zaf::ActivateOptions::NoMouseActivate, false);
    test(zaf::ActivateOptions::DiscardMouseMessage, false);
}


TEST_F(WindowTest, NoAutoActivate_Show) {

    auto test = [](bool no_auto_activate) {

        auto window = zaf::Create<zaf::Window>();
        window->SetActivateOptions(
            no_auto_activate ? 
            zaf::ActivateOptions::NoAutoActivate : 
            zaf::ActivateOptions::Normal);

        window->Show();
        HWND active_window = GetActiveWindow();
        if (no_auto_activate) {
            ASSERT_NE(window->Handle(), active_window);
        }
        else {
            ASSERT_EQ(window->Handle(), active_window);
        }
        window->Destroy();
    };
    test(true);
    test(false);
}

}