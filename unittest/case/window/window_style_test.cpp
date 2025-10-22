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


TEST_F(WindowTest, IsSizable_DefaultValue) {
    auto window = zaf::Create<zaf::Window>();
    ASSERT_TRUE(window->IsSizable());
}


TEST_F(WindowTest, IsSizable_InVariantStates) {

    auto test = [](bool expected) {

        auto window = zaf::Create<zaf::Window>();
        window->SetIsSizable(expected);

        // NotCreated
        ASSERT_EQ(window->IsSizable(), expected);

        // Creating
        bool creating_value{};
        auto sub1 = window->HandleCreatingEvent().Subscribe(
            [&](const zaf::HandleCreatingInfo& event_info) {
                creating_value = window->IsSizable();
            });
        auto holder = window->CreateHandle();
        ASSERT_EQ(creating_value, expected);

        // Created
        ASSERT_EQ(window->IsSizable(), expected);

        // Destroying
        bool destroying_value{};
        auto sub2 = window->DestroyingEvent().Subscribe(
            [&](const zaf::DestroyingInfo& event_info) {
                destroying_value = window->IsSizable();
            });
        window->Destroy();
        ASSERT_EQ(destroying_value, expected);

        // Destroyed
        ASSERT_EQ(window->IsSizable(), false);
    };
    test(true);
    test(false);
}


TEST_F(WindowTest, SetIsSizable_InVariantState) {

    auto test = [](bool expected) {
        
        // NotCreated
        {
            auto window = zaf::Create<zaf::Window>();
            window->SetIsSizable(expected);
            ASSERT_EQ(window->IsSizable(), expected);
        }

        // Creating
        {
            auto window = zaf::Create<zaf::Window>();
            bool creating_value{};
            auto sub = window->HandleCreatingEvent().Subscribe(
                [&](const zaf::HandleCreatingInfo& event_info) {
                    window->SetIsSizable(expected);
                    creating_value = window->IsSizable();
                });
            auto holder = window->CreateHandle();
            ASSERT_EQ(creating_value, expected);
            ASSERT_EQ(window->IsSizable(), expected);
            window->Destroy();
        }

        // Created
        {
            auto window = zaf::Create<zaf::Window>();
            auto holder = window->CreateHandle();
            window->SetIsSizable(expected);
            ASSERT_EQ(window->IsSizable(), expected);
            window->Destroy();
        }

        // Destroying
        {
            auto window = zaf::Create<zaf::Window>();
            auto holder = window->CreateHandle();
            bool destroying_value{};
            auto sub = window->DestroyingEvent().Subscribe(
                [&](const zaf::DestroyingInfo& event_info) {
                    window->SetIsSizable(expected);
                    destroying_value = window->IsSizable();
                });
            window->Destroy();
            ASSERT_EQ(destroying_value, expected);
        }

        // Destroyed
        {
            auto window = zaf::Create<zaf::Window>();
            auto holder = window->CreateHandle();
            window->Destroy();
            ASSERT_THROW(window->SetIsSizable(expected), zaf::InvalidHandleStateError);
            ASSERT_EQ(window->IsSizable(), false);
        }
    };
    test(true);
    test(false);
}


TEST_F(WindowTest, SetIsSizable_InvalidOperation) {
    auto window = zaf::Create<zaf::Window>();
    window->SetIsPopup(true);
    window->SetHasBorder(false);
    ASSERT_THROW(window->SetIsSizable(true), zaf::InvalidOperationError);
}


TEST_F(WindowTest, CanMaximize_DefaultValue) {
    auto window = zaf::Create<zaf::Window>();
    ASSERT_TRUE(window->CanMaximize());
}


TEST_F(WindowTest, CanMaximize_InVariantStates) {

    auto test = [](bool expected) {

        auto window = zaf::Create<zaf::Window>();
        window->SetCanMaximize(expected);

        // NotCreated
        ASSERT_EQ(window->CanMaximize(), expected);

        // Creating
        bool creating_value{};
        auto sub1 = window->HandleCreatingEvent().Subscribe(
            [&](const zaf::HandleCreatingInfo& event_info) {
                creating_value = window->CanMaximize();
            });
        auto holder = window->CreateHandle();
        ASSERT_EQ(creating_value, expected);

        // Created
        ASSERT_EQ(window->CanMaximize(), expected);

        // Destroying
        bool destroying_value{};
        auto sub2 = window->DestroyingEvent().Subscribe(
            [&](const zaf::DestroyingInfo& event_info) {
                destroying_value = window->CanMaximize();
            });
        window->Destroy();
        ASSERT_EQ(destroying_value, expected);

        // Destroyed
        ASSERT_EQ(window->CanMaximize(), false);
    };

    test(true);
    test(false);
}


TEST_F(WindowTest, SetCanMaximize_InVariantStates) {

    auto test = [](bool expected) {
        
        // NotCreated
        {
            auto window = zaf::Create<zaf::Window>();
            window->SetCanMaximize(expected);
            ASSERT_EQ(window->CanMaximize(), expected);
        }

        // Creating
        {
            auto window = zaf::Create<zaf::Window>();
            bool creating_value{};
            auto sub = window->HandleCreatingEvent().Subscribe(
                [&](const zaf::HandleCreatingInfo& event_info) {
                    window->SetCanMaximize(expected);
                    creating_value = window->CanMaximize();
                });
            auto holder = window->CreateHandle();
            ASSERT_EQ(creating_value, expected);
            ASSERT_EQ(window->CanMaximize(), expected);
            window->Destroy();
        }

        // Created
        {
            auto window = zaf::Create<zaf::Window>();
            auto holder = window->CreateHandle();
            window->SetCanMaximize(expected);
            ASSERT_EQ(window->CanMaximize(), expected);
            window->Destroy();
        }

        // Destroying
        {
            auto window = zaf::Create<zaf::Window>();
            auto holder = window->CreateHandle();
            bool destroying_value{};
            auto sub = window->DestroyingEvent().Subscribe(
                [&](const zaf::DestroyingInfo& event_info) {
                    window->SetCanMaximize(expected);
                    destroying_value = window->CanMaximize();
                });
            window->Destroy();
            ASSERT_EQ(destroying_value, expected);
            ASSERT_EQ(window->CanMaximize(), false);
        }

        // Destroyed
        {
            auto window = zaf::Create<zaf::Window>();
            auto holder = window->CreateHandle();
            window->Destroy();
            ASSERT_THROW(window->SetCanMaximize(expected), zaf::InvalidHandleStateError);
            ASSERT_EQ(window->CanMaximize(), false);
        }
    };

    test(true);
    test(false);
}


TEST_F(WindowTest, CanMinimize_DefaultValue) {
    auto window = zaf::Create<zaf::Window>();
    ASSERT_TRUE(window->CanMinimize());
}


TEST_F(WindowTest, CanMinimize_InVariantStates) {

    auto test = [](bool expected) {

        auto window = zaf::Create<zaf::Window>();
        window->SetCanMinimize(expected);

        // NotCreated
        ASSERT_EQ(window->CanMinimize(), expected);

        // Creating
        bool creating_value{};
        auto sub1 = window->HandleCreatingEvent().Subscribe(
            [&](const zaf::HandleCreatingInfo& event_info) {
                creating_value = window->CanMinimize();
            });
        auto holder = window->CreateHandle();
        ASSERT_EQ(creating_value, expected);

        // Created
        ASSERT_EQ(window->CanMinimize(), expected);

        // Destroying
        bool destroying_value{};
        auto sub2 = window->DestroyingEvent().Subscribe(
            [&](const zaf::DestroyingInfo& event_info) {
                destroying_value = window->CanMinimize();
            });
        window->Destroy();
        ASSERT_EQ(destroying_value, expected);

        // Destroyed
        ASSERT_EQ(window->CanMinimize(), false);
    };

    test(true);
    test(false);
}


TEST_F(WindowTest, SetCanMinimize_InVariantStates) {

    auto test = [](bool expected) {
        
        // NotCreated
        {
            auto window = zaf::Create<zaf::Window>();
            window->SetCanMinimize(expected);
            ASSERT_EQ(window->CanMinimize(), expected);
        }

        // Creating
        {
            auto window = zaf::Create<zaf::Window>();
            bool creating_value{};
            auto sub = window->HandleCreatingEvent().Subscribe(
                [&](const zaf::HandleCreatingInfo& event_info) {
                    window->SetCanMinimize(expected);
                    creating_value = window->CanMinimize();
                });
            auto holder = window->CreateHandle();
            ASSERT_EQ(creating_value, expected);
            ASSERT_EQ(window->CanMinimize(), expected);
            window->Destroy();
        }

        // Created
        {
            auto window = zaf::Create<zaf::Window>();
            auto holder = window->CreateHandle();
            window->SetCanMinimize(expected);
            ASSERT_EQ(window->CanMinimize(), expected);
            window->Destroy();
        }

        // Destroying
        {
            auto window = zaf::Create<zaf::Window>();
            auto holder = window->CreateHandle();
            bool destroying_value{};
            auto sub = window->DestroyingEvent().Subscribe(
                [&](const zaf::DestroyingInfo& event_info) {
                    window->SetCanMinimize(expected);
                    destroying_value = window->CanMinimize();
                });
            window->Destroy();
            ASSERT_EQ(destroying_value, expected);
            ASSERT_EQ(window->CanMinimize(), false);
        }

        // Destroyed
        {
            auto window = zaf::Create<zaf::Window>();
            auto holder = window->CreateHandle();
            window->Destroy();
            ASSERT_THROW(window->SetCanMinimize(expected), zaf::InvalidHandleStateError);
            ASSERT_EQ(window->CanMinimize(), false);
        }
    };

    test(true);
    test(false);
}

}