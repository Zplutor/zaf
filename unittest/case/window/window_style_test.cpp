#include <gtest/gtest.h>
#include <zaf/window/invalid_handle_state_error.h>
#include <zaf/window/window.h>
#include "window_test.h"

namespace zaf::testing {

TEST_F(WindowTest, IsPopup_NotCreatedState) {

    auto window = zaf::Create<zaf::Window>();
    ASSERT_FALSE(window->IsPopup());

    window->SetIsPopup(true);
    ASSERT_TRUE(window->IsPopup());

    window->SetIsPopup(false);
    ASSERT_FALSE(window->IsPopup());
}


TEST_F(WindowTest, IsPopup_CreatingState) {

    auto window = zaf::Create<zaf::Window>();
    window->SetIsPopup(true);

    bool is_popup_in_event{};
    auto sub = window->HandleCreatingEvent().Subscribe(
        [&](const zaf::HandleCreatingInfo& event_info) {
            is_popup_in_event = window->IsPopup();
        });

    auto holder = window->CreateHandle();
    ASSERT_TRUE(is_popup_in_event);
    window->Destroy();
}


TEST_F(WindowTest, IsPopup_CreatedState) {

    {
        auto window = zaf::Create<zaf::Window>();
        window->SetIsPopup(true);

        auto holder = window->CreateHandle();
        ASSERT_TRUE(window->IsPopup());
        window->Destroy();
    }

    {
        auto window = zaf::Create<zaf::Window>();
        window->SetIsPopup(false);
        auto holder = window->CreateHandle();
        ASSERT_FALSE(window->IsPopup());
        window->Destroy();
    }
}


TEST_F(WindowTest, IsPopup_DestroyingState) {

    auto window = zaf::Create<zaf::Window>();
    window->SetIsPopup(true);

    bool is_popup_in_event{};
    auto sub = window->DestroyingEvent().Subscribe(
        [&](const zaf::DestroyingInfo& event_info) {
            is_popup_in_event = window->IsPopup();
        });

    auto holder = window->CreateHandle();
    window->Destroy();
    ASSERT_TRUE(is_popup_in_event);
}


TEST_F(WindowTest, IsPopup_DestroyedState) {
    auto window = zaf::Create<zaf::Window>();
    window->SetIsPopup(true);
    auto holder = window->CreateHandle();
    window->Destroy();
    ASSERT_FALSE(window->IsPopup());
}


TEST_F(WindowTest, SetIsPopup_ChangeDerivedProperties) {

    auto window = zaf::Create<zaf::Window>();
    window->SetIsPopup(true);
    window->SetHasBorder(false);
    window->SetHasTitleBar(false);

    window->SetIsPopup(false);
    ASSERT_TRUE(window->HasBorder());
    ASSERT_TRUE(window->HasTitleBar());
}


TEST_F(WindowTest, SetIsPopup_InvalidStates) {

    // Creating
    {
        auto window = zaf::Create<zaf::Window>();
        bool has_asserted{};
        auto sub = window->HandleCreatingEvent().Subscribe(
            [&](const zaf::HandleCreatingInfo& event_info) {
                has_asserted = true;
                ASSERT_THROW(window->SetIsPopup(true), zaf::InvalidHandleStateError);
            });
        auto holder = window->CreateHandle();
        ASSERT_TRUE(has_asserted);
    }

    // Created
    {
        auto window = zaf::Create<zaf::Window>();
        auto holder = window->CreateHandle();
        ASSERT_THROW(window->SetIsPopup(true), zaf::InvalidHandleStateError);
        window->Destroy();
    }

    // Destroying/Destroyed
    {
        auto window = zaf::Create<zaf::Window>();
        auto holder = window->CreateHandle();
        bool has_asserted{};
        auto sub = window->DestroyingEvent().Subscribe(
            [&](const zaf::DestroyingInfo& event_info) {
                has_asserted = true;
                ASSERT_THROW(window->SetIsPopup(true), zaf::InvalidHandleStateError);
            });
        window->Destroy();
        ASSERT_TRUE(has_asserted);
        ASSERT_THROW(window->SetIsPopup(true), zaf::InvalidHandleStateError);
    }
}


TEST_F(WindowTest, HasBorder_NotCreatedState) {

    auto window = zaf::Create<zaf::Window>();
    window->SetIsPopup(true);
    ASSERT_TRUE(window->HasBorder());

    window->SetHasBorder(false);
    ASSERT_FALSE(window->HasBorder());

    window->SetHasBorder(true);
    ASSERT_TRUE(window->HasBorder());
}


TEST_F(WindowTest, HasBorder_CreatingState) {

    auto window = zaf::Create<zaf::Window>();
    window->SetIsPopup(true);
    window->SetHasBorder(false);

    bool has_border_in_event{};
    auto sub = window->HandleCreatingEvent().Subscribe(
        [&](const zaf::HandleCreatingInfo& event_info) {
            has_border_in_event = window->HasBorder();
        });

    auto holder = window->CreateHandle();
    ASSERT_FALSE(has_border_in_event);
    window->Destroy();
}


TEST_F(WindowTest, HasBorder_CreatedState) {

    // Overlapped window.
    {
        auto window = zaf::Create<zaf::Window>();
        window->SetIsPopup(false);
        window->SetHasBorder(true);
        auto holder = window->CreateHandle();
        ASSERT_TRUE(window->HasBorder());
        window->Destroy();
    }

    // Popup window.
    {
        auto window = zaf::Create<zaf::Window>();
        window->SetIsPopup(true);
        window->SetHasBorder(true);
        auto holder = window->CreateHandle();
        ASSERT_TRUE(window->HasBorder());
        window->Destroy();
    }

    // Popup window without border.
    {
        auto window = zaf::Create<zaf::Window>();
        window->SetIsPopup(true);
        window->SetHasBorder(false);
        auto holder = window->CreateHandle();
        ASSERT_FALSE(window->HasBorder());
        window->Destroy();
    }
}


TEST_F(WindowTest, HasBorder_DestroyingState) {

    auto window = zaf::Create<zaf::Window>();
    window->SetIsPopup(true);
    window->SetHasBorder(false);

    bool has_border_in_event{};
    auto sub = window->DestroyingEvent().Subscribe(
        [&](const zaf::DestroyingInfo& event_info) {
            has_border_in_event = window->HasBorder();
        });

    auto holder = window->CreateHandle();
    window->Destroy();
    ASSERT_FALSE(has_border_in_event);
}


TEST_F(WindowTest, HasBorder_DestroyedState) {
    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    window->Destroy();
    ASSERT_FALSE(window->HasBorder());
}


TEST_F(WindowTest, SetHasBorder_ChangeDerivedProperties) {

    auto window = zaf::Create<zaf::Window>();
    window->SetIsPopup(true);
    window->SetHasTitleBar(true);
    window->SetHasSystemMenu(true);
    window->SetIsSizable(true);

    window->SetHasBorder(false);
    ASSERT_FALSE(window->HasTitleBar());
    ASSERT_FALSE(window->HasSystemMenu());
    ASSERT_FALSE(window->IsSizable());
}


TEST_F(WindowTest, SetHasBorder_InvalidOperation) {

    auto window = zaf::Create<zaf::Window>();
    ASSERT_NO_THROW(window->SetHasBorder(true));
    ASSERT_THROW(window->SetHasBorder(false), zaf::InvalidOperationError);
}


TEST_F(WindowTest, SetHasBorder_InvalidStates) {

    // Creating
    {
        auto window = zaf::Create<zaf::Window>();
        bool has_asserted{};
        auto sub = window->HandleCreatingEvent().Subscribe(
            [&](const zaf::HandleCreatingInfo& event_info) {
                has_asserted = true;
                ASSERT_THROW(window->SetHasBorder(true), zaf::InvalidHandleStateError);
            });
        auto holder = window->CreateHandle();
        ASSERT_TRUE(has_asserted);
    }

    // Created
    {
        auto window = zaf::Create<zaf::Window>();
        auto holder = window->CreateHandle();
        ASSERT_THROW(window->SetHasBorder(true), zaf::InvalidHandleStateError);
        window->Destroy();
    }

    // Destroying/Destroyed
    {
        auto window = zaf::Create<zaf::Window>();
        auto holder = window->CreateHandle();
        bool has_asserted{};
        auto sub = window->DestroyingEvent().Subscribe(
            [&](const zaf::DestroyingInfo& event_info) {
                has_asserted = true;
                ASSERT_THROW(window->SetHasBorder(true), zaf::InvalidHandleStateError);
            });
        window->Destroy();
        ASSERT_TRUE(has_asserted);
        ASSERT_THROW(window->SetHasBorder(true), zaf::InvalidHandleStateError);
    }
}


TEST_F(WindowTest, HasTitleBar_NotCreatedState) {

    auto window = zaf::Create<zaf::Window>();
    ASSERT_TRUE(window->HasTitleBar());

    window->SetIsPopup(true);
    window->SetHasTitleBar(false);
    ASSERT_FALSE(window->HasTitleBar());

    window->SetHasTitleBar(true);
    ASSERT_TRUE(window->HasTitleBar());
}


TEST_F(WindowTest, HasTitleBar_CreatingState) {

    auto window = zaf::Create<zaf::Window>();
    window->SetIsPopup(true);
    window->SetHasTitleBar(false);
    bool has_title_bar_in_event{};
    auto sub = window->HandleCreatingEvent().Subscribe(
        [&](const zaf::HandleCreatingInfo& event_info) {
            has_title_bar_in_event = window->HasTitleBar();
        });
    auto holder = window->CreateHandle();
    ASSERT_FALSE(has_title_bar_in_event);
    window->Destroy();
}


TEST_F(WindowTest, HasTitleBar_CreatedState) {

    // Overlapped window.
    {
        auto window = zaf::Create<zaf::Window>();
        window->SetIsPopup(false);
        window->SetHasTitleBar(true);
        auto holder = window->CreateHandle();
        ASSERT_TRUE(window->HasTitleBar());
        window->Destroy();
    }

    // Popup window.
    {
        auto window = zaf::Create<zaf::Window>();
        window->SetIsPopup(true);
        window->SetHasTitleBar(true);
        auto holder = window->CreateHandle();
        ASSERT_TRUE(window->HasTitleBar());
        window->Destroy();
    }

    // Popup window without title bar.
    {
        auto window = zaf::Create<zaf::Window>();
        window->SetIsPopup(true);
        window->SetHasTitleBar(false);
        auto holder = window->CreateHandle();
        ASSERT_FALSE(window->HasTitleBar());
        window->Destroy();
    }
}


TEST_F(WindowTest, HasTitleBar_DestroyingState) {
    auto window = zaf::Create<zaf::Window>();
    window->SetIsPopup(true);
    window->SetHasTitleBar(false);
    bool has_title_bar_in_event{};
    auto sub = window->DestroyingEvent().Subscribe(
        [&](const zaf::DestroyingInfo& event_info) {
            has_title_bar_in_event = window->HasTitleBar();
        });
    auto holder = window->CreateHandle();
    window->Destroy();
    ASSERT_FALSE(has_title_bar_in_event);
}


TEST_F(WindowTest, HasTitleBar_DestroyedState) {
    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    window->Destroy();
    ASSERT_FALSE(window->HasTitleBar());
}


// Make sure that setting HasTitleBar to false won't affect HasBorder property.
// Because the underlying window style WS_CAPTION includes WS_BORDER.
TEST_F(WindowTest, SetHasTitleBar_InfluenceHasBorder) {

    {
        auto window = zaf::Create<zaf::Window>();
        window->SetIsPopup(true);
        window->SetHasBorder(true);
        window->SetHasTitleBar(false);
        ASSERT_TRUE(window->HasBorder());
    }

    {
        auto window = zaf::Create<zaf::Window>();
        window->SetIsPopup(true);
        window->SetHasBorder(false);
        window->SetHasTitleBar(false);
        ASSERT_FALSE(window->HasBorder());
    }
}


TEST_F(WindowTest, SetHasTitleBar_ChangeDerivedProperties) {

    auto window = zaf::Create<zaf::Window>();
    window->SetIsPopup(true);
    window->SetHasBorder(true);
    window->SetHasTitleBar(true);
    window->SetHasSystemMenu(true);

    window->SetHasTitleBar(false);
    ASSERT_FALSE(window->HasSystemMenu());
}


TEST_F(WindowTest, SetHasTitleBar_InvalidOperation) {

    // Overlapped window.
    {
        auto window = zaf::Create<zaf::Window>();
        ASSERT_NO_THROW(window->SetHasTitleBar(true));
        ASSERT_THROW(window->SetHasTitleBar(false), zaf::InvalidOperationError);
    }

    // No border window.
    {
        auto window = zaf::Create<zaf::Window>();
        window->SetIsPopup(true);
        window->SetHasBorder(false);
        ASSERT_NO_THROW(window->SetHasTitleBar(false));
        ASSERT_THROW(window->SetHasTitleBar(true), zaf::InvalidOperationError);
    }
}


TEST_F(WindowTest, SetHasTitleBar_InvalidStates) {

    // Creating
    {
        auto window = zaf::Create<zaf::Window>();
        bool has_asserted{};
        auto sub = window->HandleCreatingEvent().Subscribe(
            [&](const zaf::HandleCreatingInfo& event_info) {
                has_asserted = true;
                ASSERT_THROW(window->SetHasTitleBar(true), zaf::InvalidHandleStateError);
            });
        auto holder = window->CreateHandle();
        ASSERT_TRUE(has_asserted);
    }

    // Created
    {
        auto window = zaf::Create<zaf::Window>();
        auto holder = window->CreateHandle();
        ASSERT_THROW(window->SetHasTitleBar(true), zaf::InvalidHandleStateError);
        window->Destroy();
    }

    // Destroying/Destroyed
    {
        auto window = zaf::Create<zaf::Window>();
        auto holder = window->CreateHandle();
        bool has_asserted{};
        auto sub = window->DestroyingEvent().Subscribe(
            [&](const zaf::DestroyingInfo& event_info) {
                has_asserted = true;
                ASSERT_THROW(window->SetHasTitleBar(true), zaf::InvalidHandleStateError);
            });
        window->Destroy();
        ASSERT_TRUE(has_asserted);
        ASSERT_THROW(window->SetHasTitleBar(true), zaf::InvalidHandleStateError);
    }
}


TEST_F(WindowTest, HasSystemMenu_NotCreatedState) {

    auto window = zaf::Create<zaf::Window>();
    ASSERT_TRUE(window->HasSystemMenu());

    window->SetHasSystemMenu(false);
    ASSERT_FALSE(window->HasSystemMenu());

    window->SetHasSystemMenu(true);
    ASSERT_TRUE(window->HasSystemMenu());
}


TEST_F(WindowTest, HasSystemMenu_CreatingState) {

    auto window = zaf::Create<zaf::Window>();
    window->SetHasSystemMenu(false);

    bool has_system_menu_in_event{};
    auto sub = window->HandleCreatingEvent().Subscribe(
        [&](const zaf::HandleCreatingInfo& event_info) {
            has_system_menu_in_event = window->HasSystemMenu();
        });
    auto holder = window->CreateHandle();
    ASSERT_FALSE(has_system_menu_in_event);
    window->Destroy();
}


TEST_F(WindowTest, HasSystemMenu_CreatedState) {

    {
        auto window = zaf::Create<zaf::Window>();
        window->SetHasSystemMenu(true);
        auto holder = window->CreateHandle();
        ASSERT_TRUE(window->HasSystemMenu());
        window->Destroy();
    }

    {
        auto window = zaf::Create<zaf::Window>();
        window->SetHasSystemMenu(false);
        auto holder = window->CreateHandle();
        ASSERT_FALSE(window->HasSystemMenu());
        window->Destroy();
    }
}


TEST_F(WindowTest, HasSystemMenu_DestroyingState) {

    auto window = zaf::Create<zaf::Window>();
    window->SetHasSystemMenu(false);
    bool has_system_menu_in_event{};
    auto sub = window->DestroyingEvent().Subscribe(
        [&](const zaf::DestroyingInfo& event_info) {
            has_system_menu_in_event = window->HasSystemMenu();
        });
    auto holder = window->CreateHandle();
    window->Destroy();
    ASSERT_FALSE(has_system_menu_in_event);
}


TEST_F(WindowTest, HasSystemMenu_DestroyedState) {
    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    window->Destroy();
    ASSERT_FALSE(window->HasSystemMenu());
}


TEST_F(WindowTest, SetHasSystemMenu_InvalidOperation) {

    auto window = zaf::Create<zaf::Window>();
    window->SetIsPopup(true);
    window->SetHasTitleBar(false);
    ASSERT_THROW(window->SetHasSystemMenu(true), zaf::InvalidOperationError);
}


TEST_F(WindowTest, SetHasSystemMenu_InvalidStates) {

    // Creating
    {
        auto window = zaf::Create<zaf::Window>();
        bool has_asserted{};
        auto sub = window->HandleCreatingEvent().Subscribe(
            [&](const zaf::HandleCreatingInfo& event_info) {
                has_asserted = true;
                ASSERT_THROW(window->SetHasSystemMenu(true), zaf::InvalidHandleStateError);
            });
        auto holder = window->CreateHandle();
        ASSERT_TRUE(has_asserted);
    }

    // Created
    {
        auto window = zaf::Create<zaf::Window>();
        auto holder = window->CreateHandle();
        ASSERT_THROW(window->SetHasSystemMenu(true), zaf::InvalidHandleStateError);
        window->Destroy();
    }

    // Destroying/Destroyed
    {
        auto window = zaf::Create<zaf::Window>();
        auto holder = window->CreateHandle();
        bool has_asserted{};
        auto sub = window->DestroyingEvent().Subscribe(
            [&](const zaf::DestroyingInfo& event_info) {
                has_asserted = true;
                ASSERT_THROW(window->SetHasSystemMenu(true), zaf::InvalidHandleStateError);
            });
        window->Destroy();
        ASSERT_TRUE(has_asserted);
        ASSERT_THROW(window->SetHasSystemMenu(true), zaf::InvalidHandleStateError);
    }
}


TEST_F(WindowTest, IsSizable_NotCreatedState) {

    auto window = zaf::Create<zaf::Window>();
    ASSERT_TRUE(window->IsSizable());

    window->SetIsSizable(false);
    ASSERT_FALSE(window->IsSizable());

    window->SetIsSizable(true);
    ASSERT_TRUE(window->IsSizable());
}


TEST_F(WindowTest, IsSizable_CreatingState) {

    auto window = zaf::Create<zaf::Window>();
    window->SetIsSizable(false);
    bool is_sizable_in_event{};
    auto sub = window->HandleCreatingEvent().Subscribe(
        [&](const zaf::HandleCreatingInfo& event_info) {
            is_sizable_in_event = window->IsSizable();
        });
    auto holder = window->CreateHandle();
    ASSERT_FALSE(is_sizable_in_event);
    window->Destroy();
}


TEST_F(WindowTest, IsSizable_CreatedState) {

    {
        auto window = zaf::Create<zaf::Window>();
        window->SetIsSizable(true);
        auto holder = window->CreateHandle();
        ASSERT_TRUE(window->IsSizable());
        window->Destroy();
    }

    {
        auto window = zaf::Create<zaf::Window>();
        window->SetIsSizable(false);
        auto holder = window->CreateHandle();
        ASSERT_FALSE(window->IsSizable());
        window->Destroy();
    }
}


TEST_F(WindowTest, IsSizable_DestroyingState) {
    auto window = zaf::Create<zaf::Window>();
    window->SetIsSizable(false);
    bool is_sizable_in_event{};
    auto sub = window->DestroyingEvent().Subscribe(
        [&](const zaf::DestroyingInfo& event_info) {
            is_sizable_in_event = window->IsSizable();
        });
    auto holder = window->CreateHandle();
    window->Destroy();
    ASSERT_FALSE(is_sizable_in_event);
}


TEST_F(WindowTest, IsSizable_DestroyedState) {
    auto window = zaf::Create<zaf::Window>();
    window->SetIsSizable(true);
    auto holder = window->CreateHandle();
    window->Destroy();
    ASSERT_FALSE(window->IsSizable());
}


TEST_F(WindowTest, SetIsSizable_CreatingState) {

    auto window = zaf::Create<zaf::Window>();
    window->SetIsSizable(true);

    auto sub = window->HandleCreatingEvent().Subscribe(
        [&](const zaf::HandleCreatingInfo& event_info) {
            window->SetIsSizable(false);
        });

    auto holder = window->CreateHandle();
    ASSERT_FALSE(window->IsSizable());
    window->Destroy();
}


TEST_F(WindowTest, SetIsSizable_CreatedState) {

    auto window = zaf::Create<zaf::Window>();
    window->SetIsSizable(true);
    auto holder = window->CreateHandle();

    window->SetIsSizable(false);
    ASSERT_FALSE(window->IsSizable());

    window->SetIsSizable(true);
    ASSERT_TRUE(window->IsSizable());

    window->Destroy();
}


TEST_F(WindowTest, SetIsSizable_DestroyingState) {

    auto window = zaf::Create<zaf::Window>();
    window->SetIsSizable(true);

    auto holder = window->CreateHandle();
    bool has_asserted{};
    auto sub = window->DestroyingEvent().Subscribe(
        [&](const zaf::DestroyingInfo& event_info) {
            ASSERT_NO_THROW(window->SetIsSizable(false));
            ASSERT_FALSE(window->IsSizable());
            has_asserted = true;
        });
    
    window->Destroy();
    ASSERT_TRUE(has_asserted);
}


TEST_F(WindowTest, SetIsSizable_DestroyedState) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    window->Destroy();
    ASSERT_THROW(window->SetIsSizable(true), zaf::InvalidHandleStateError);
}


TEST_F(WindowTest, SetIsSizable_InvalidOperation) {
    auto window = zaf::Create<zaf::Window>();
    window->SetIsPopup(true);
    window->SetHasBorder(false);
    ASSERT_THROW(window->SetIsSizable(true), zaf::InvalidOperationError);
}

}