#include <gtest/gtest.h>
#include <zaf/application.h>
#include <zaf/base/as.h>
#include <zaf/control/label.h>
#include <zaf/creation.h>
#include <zaf/graphic/pixel_snapping.h>
#include <zaf/graphic/dpi.h>
#include <zaf/window/invalid_handle_state_error.h>
#include <zaf/window/window.h>
#include <zaf/window/window_class.h>
#include <zaf/window/window_class_registry.h>
#include <zaf/window/window_holder.h>
#include "window_test.h"

namespace zaf::testing {

TEST_F(WindowTest, ClassName) {

    //Default class name
    auto window = zaf::Create<zaf::Window>();
    ASSERT_EQ(window->Class()->Name(), zaf::WindowClassRegistry::DefaultWindowClassName);

    auto holder = window->CreateHandle();
    wchar_t buffer[100]{};
    GetClassName(window->Handle(), buffer, 100);
    ASSERT_EQ(std::wcscmp(buffer, zaf::WindowClassRegistry::DefaultWindowClassName), 0);

    //Custom class name
    const std::wstring custom_class_name(L"CustomClassName");
    window = zaf::Create<zaf::Window>(custom_class_name);
    ASSERT_EQ(window->Class()->Name(), custom_class_name);

    holder = window->CreateHandle();
    GetClassName(window->Handle(), buffer, 100);
    ASSERT_EQ(std::wcscmp(buffer, custom_class_name.c_str()), 0);
}


TEST_F(WindowTest, CreateHandle_NotCreatedState) {

    auto window = zaf::Create<zaf::Window>();

    auto holder = window->CreateHandle();
    ASSERT_NE(holder, nullptr);
    ASSERT_NE(window->Handle(), nullptr);
}


TEST_F(WindowTest, CreateHandle_CreatedState) {

    auto window = zaf::Create<zaf::Window>();
    auto holder1 = window->CreateHandle();
    auto holder2 = window->CreateHandle();
    ASSERT_NE(holder2, nullptr);
    ASSERT_EQ(holder1, holder2);
}


TEST_F(WindowTest, CreateHandle_CreatingState) {

    auto window = zaf::Create<zaf::Window>();
    bool has_asserted{};
    auto sub = window->HandleCreatingEvent().Subscribe(
        [&](const zaf::HandleCreatingInfo& event_info) {
            ASSERT_THROW(auto holder = window->CreateHandle(), zaf::InvalidHandleStateError);
            has_asserted = true;
        });
    auto holder = window->CreateHandle();
    ASSERT_TRUE(has_asserted);
}


TEST_F(WindowTest, CreateHandle_ThrowInHandleCreatedEvent) {

    auto window = zaf::Create<zaf::Window>();
    auto sub = window->HandleCreatedEvent().Subscribe(
        [](const zaf::HandleCreatedInfo& event_info) {
            throw zaf::InvalidOperationError{};
        });

    ASSERT_THROW(auto holder = window->CreateHandle(), zaf::InvalidOperationError);
    ASSERT_EQ(window->HandleState(), zaf::WindowHandleState::NotCreated);
    ASSERT_EQ(window->Handle(), nullptr);
}


TEST_F(WindowTest, CreateHandle_WithOwner) {

    // Owner in Creating state
    {
        auto owner = zaf::Create<zaf::Window>();
        std::shared_ptr<WindowHolder> holder;
        auto sub = owner->HandleCreatingEvent().Subscribe(
            [&](const zaf::HandleCreatingInfo& event_info) {
                auto window = zaf::Create<zaf::Window>();
                window->SetOwner(owner);
                holder = window->CreateHandle();
            });

        auto owner_holder = owner->CreateHandle();
        ASSERT_NE(holder, nullptr);
        ASSERT_EQ(GetWindow(holder->Window()->Handle(), GW_OWNER), owner->Handle());
    }

    // Owner in Created state
    {
        auto owner = zaf::Create<zaf::Window>();
        auto owner_holder = owner->CreateHandle();

        auto window = zaf::Create<zaf::Window>();
        window->SetOwner(owner);

        auto holder = window->CreateHandle();
        ASSERT_EQ(GetWindow(holder->Window()->Handle(), GW_OWNER), owner->Handle());
    }
}


TEST_F(WindowTest, CreateHandle_InvalidOwnerState) {

    //Owner in NotCreated state
    {
        auto owner = zaf::Create<zaf::Window>();
        auto window = zaf::Create<zaf::Window>();
        window->SetOwner(owner);
        ASSERT_THROW(auto holder = window->CreateHandle(), zaf::InvalidHandleStateError);
    }

    // Owner in Destroying state
    {
        auto owner = zaf::Create<zaf::Window>();
        auto owner_holder = owner->CreateHandle();
        bool has_asserted{};
        auto sub = owner->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo& event_info) {
            
            auto window = zaf::Create<zaf::Window>();
            window->SetOwner(owner);
            ASSERT_THROW(auto holder = window->CreateHandle(), zaf::InvalidHandleStateError);
            has_asserted = true;
        });
        owner->Destroy();
        ASSERT_TRUE(has_asserted);
    }

    //Owner in Destroyed state
    {
        auto owner = zaf::Create<zaf::Window>();
        auto owner_holder = owner->CreateHandle();
        owner_holder.reset();

        auto window = zaf::Create<zaf::Window>();
        window->SetOwner(owner);
        ASSERT_THROW(auto holder = window->CreateHandle(), zaf::InvalidHandleStateError);
    }
}


TEST_F(WindowTest, HandleCreatingEvent_HandleState) {

    auto window = zaf::Create<zaf::Window>();
    zaf::WindowHandleState state_in_event{};
    auto sub = window->HandleCreatingEvent().Subscribe(
        [&](const zaf::HandleCreatingInfo& event_info) {
            // When this event is raised, the handle state should be Creating.
            state_in_event = window->HandleState();
        });
    auto holder = window->CreateHandle();
    ASSERT_EQ(state_in_event, zaf::WindowHandleState::Creating);
    window->Destroy();
}


TEST_F(WindowTest, HandleCreatedEvent_HandleState) {

    auto window = zaf::Create<zaf::Window>();
    zaf::WindowHandleState state_in_event{};
    auto sub = window->HandleCreatedEvent().Subscribe(
        [&](const zaf::HandleCreatedInfo& event_info) {
            // When this event is raised, the handle state should be Created.
            state_in_event = window->HandleState();
        });
    auto holder = window->CreateHandle();
    ASSERT_EQ(state_in_event, zaf::WindowHandleState::Created);
    window->Destroy();
}


TEST_F(WindowTest, GetHandleInDifferentStates) {

    // NotCreated
    {
        auto window = zaf::Create<zaf::Window>();
        ASSERT_EQ(window->Handle(), nullptr);
    }

    // Creating
    {
        auto window = zaf::Create<zaf::Window>();
        HWND handle{};
        auto sub = window->HandleCreatingEvent().Subscribe(
            [&](const zaf::HandleCreatingInfo& event_info) {
                handle = window->Handle();
            });
        auto holder = window->CreateHandle();
        ASSERT_NE(handle, nullptr);
        window->Destroy();
    }

    // Created
    {
        auto window = zaf::Create<zaf::Window>();
        auto holder = window->CreateHandle();
        ASSERT_NE(window->Handle(), nullptr);
        window->Destroy();
    }

    // Destroying
    {
        auto window = zaf::Create<zaf::Window>();
        HWND handle{};
        auto sub = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo& event_info) {
            handle = window->Handle();
        });
        auto holder = window->CreateHandle();
        window->Destroy();
        ASSERT_NE(handle, nullptr);
    }

    // Destroyed
    {
        auto window = zaf::Create<zaf::Window>();
        auto holder = window->CreateHandle();
        window->Destroy();
        ASSERT_EQ(window->Handle(), nullptr);
    }
}


TEST_F(WindowTest, Show_InNotCreatedState) {

    auto window = zaf::Create<zaf::Window>();
    window->Show();
    ASSERT_TRUE(window->IsVisible());
    ASSERT_TRUE(IsWindowRegistered(window));
    window->Destroy();
}


TEST_F(WindowTest, Show_InCreatingState) {

    {
        auto window = zaf::Create<zaf::Window>();
        auto sub = window->HandleCreatingEvent().Subscribe(
            [&window](const zaf::HandleCreatingInfo& event_info) {
                window->Show();
            });
        auto holder = window->CreateHandle();
        ASSERT_TRUE(window->IsVisible());
        ASSERT_TRUE(IsWindowRegistered(window));
        window->Destroy();
    }

    // Re-enter
    {
        auto window = zaf::Create<zaf::Window>();
        auto sub = window->HandleCreatingEvent().Subscribe(
            [&window](const zaf::HandleCreatingInfo& event_info) {
                window->Show();
            });
        window->Show();
        ASSERT_TRUE(window->IsVisible());
        ASSERT_TRUE(IsWindowRegistered(window));
        window->Destroy();
    }
}


TEST_F(WindowTest, Show_InCreatedState) {

    {
        auto window = zaf::Create<zaf::Window>();
        auto holder = window->CreateHandle();

        window->Show();
        ASSERT_TRUE(window->IsVisible());
        ASSERT_TRUE(IsWindowRegistered(window));
        window->Destroy();
    }

    // Throw after show
    {
        auto window = zaf::Create<zaf::Window>();
        auto sub1 = window->HandleCreatedEvent().Subscribe(
            [&window](const zaf::HandleCreatedInfo& event_info) {
                window->Show();
            });
        auto sub2 = window->HandleCreatedEvent().Subscribe(
            [&window](const zaf::HandleCreatedInfo& event_info) {
                throw zaf::InvalidOperationError{};
            });

        ASSERT_THROW(auto holder = window->CreateHandle(), zaf::InvalidOperationError);
        ASSERT_FALSE(window->IsVisible());
        ASSERT_FALSE(IsWindowRegistered(window));
    }
}


TEST_F(WindowTest, Show_InInvalidState) {

    //Destroyed state
    {
        auto window = zaf::Create<zaf::Window>();
        auto holder = window->CreateHandle();
        holder.reset();
        ASSERT_THROW(window->Show(), zaf::InvalidHandleStateError);
    }
}


TEST_F(WindowTest, Maximize_InNotCreatedState) {

    auto window = zaf::Create<zaf::Window>();
    window->Maximize();
    ASSERT_TRUE(window->IsVisible());
    ASSERT_TRUE(window->IsWindowMaximized());
    ASSERT_TRUE(IsWindowRegistered(window));
    window->Destroy();
}


TEST_F(WindowTest, Maximize_InCreatedState) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();

    window->Maximize();
    ASSERT_TRUE(window->IsVisible());
    ASSERT_TRUE(window->IsWindowMaximized());
    ASSERT_TRUE(IsWindowRegistered(window));
    window->Destroy();
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


TEST_F(WindowTest, Minimize_InNotCreatedState) {

    auto window = zaf::Create<zaf::Window>();
    window->Minimize();
    ASSERT_TRUE(window->IsVisible());
    ASSERT_TRUE(window->IsWindowMinimized());
    ASSERT_TRUE(IsWindowRegistered(window));
    window->Destroy();
}


TEST_F(WindowTest, Minimize_InCreatedState) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    window->Minimize();
    ASSERT_TRUE(window->IsVisible());
    ASSERT_TRUE(window->IsWindowMinimized());
    ASSERT_TRUE(IsWindowRegistered(window));
    window->Destroy();
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


TEST_F(WindowTest, Restore_InNotCreatedState) {

    auto window = zaf::Create<zaf::Window>();
    window->Restore();
    ASSERT_TRUE(window->IsVisible());
    ASSERT_FALSE(window->IsWindowMinimized());
    ASSERT_FALSE(window->IsWindowMaximized());
    ASSERT_TRUE(IsWindowRegistered(window));
    window->Destroy();
}


TEST_F(WindowTest, Restore_InCreatedState) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    window->Restore();
    ASSERT_TRUE(window->IsVisible());
    ASSERT_FALSE(window->IsWindowMinimized());
    ASSERT_FALSE(window->IsWindowMaximized());
    ASSERT_TRUE(IsWindowRegistered(window));
    window->Destroy();
}


TEST_F(WindowTest, Restore_WhenMaximized) {

    auto window = zaf::Create<zaf::Window>();
    window->Maximize();
    window->Restore();
    ASSERT_FALSE(window->IsWindowMaximized());
    window->Destroy();
}


TEST_F(WindowTest, Restore_WhenMinimized) {
    auto window = zaf::Create<zaf::Window>();
    window->Minimize();
    window->Restore();
    ASSERT_FALSE(window->IsWindowMinimized());
    window->Destroy();
}


TEST_F(WindowTest, SetRectBeforeCreate) {

    auto window = zaf::Create<zaf::Window>();
    window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);

    zaf::Rect initial_rect{ 10.25, 20.25, 400.25, 500.25 };
    window->SetRect(initial_rect);
    ASSERT_EQ(window->Rect(), initial_rect);

    auto holder = window->CreateHandle();
    //After creating the window, Rect() should use the actual window rect.
    auto rect_after_create = zaf::SnapToPixels(initial_rect, window->GetDPI());
    ASSERT_EQ(window->Rect(), rect_after_create);

    window->Destroy();
    //The rect should be reserved after window is destroyed.
    ASSERT_EQ(window->Rect(), rect_after_create);
}


TEST_F(WindowTest, SetRectAfterCreate) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();

    zaf::Rect rect{ 10.25, 20.25, 400.25, 500.25 };
    window->SetRect(rect);

    //After calling SetRect(), the rect should be adjusted to the actual window rect.
    auto rect_after_set = zaf::SnapToPixels(rect, window->GetDPI());
    ASSERT_EQ(window->Rect(), rect_after_set);

    window->Destroy();
    //The rect should be reserved after window is destroyed.
    ASSERT_EQ(window->Rect(), rect_after_set);
}


TEST_F(WindowTest, GetRectAfterChangeRect) {

    //Simulate user resize the window.
    {
        auto window = zaf::Create<zaf::Window>();
        window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
        window->SetRect(zaf::Rect{ 100, 100, 400, 400 });

        auto holder = window->CreateHandle();
        SetWindowPos(
            window->Handle(),
            nullptr,
            0,
            0,
            static_cast<int>(zaf::FromDIPs(400, window->GetDPI())),
            static_cast<int>(zaf::FromDIPs(500, window->GetDPI())),
            SWP_NOMOVE | SWP_NOACTIVATE);
        window->Destroy();

        zaf::Rect new_rect{ 100, 100, 400, 500 };
        ASSERT_EQ(window->Rect(), new_rect);
    }

    //Simulate user move the window.
    {
        auto window = zaf::Create<zaf::Window>();
        window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
        window->SetRect(zaf::Rect{ 100, 100, 400, 400 });

        auto holder = window->CreateHandle();
        SetWindowPos(
            window->Handle(),
            nullptr,
            static_cast<int>(zaf::FromDIPs(200, window->GetDPI())),
            static_cast<int>(zaf::FromDIPs(300, window->GetDPI())),
            0,
            0,
            SWP_NOSIZE | SWP_NOACTIVATE);
        window->Destroy();

        zaf::Rect new_rect{ 200, 300, 400, 400 };
        ASSERT_EQ(window->Rect(), new_rect);
    }
}


TEST_F(WindowTest, SetContentSize) {

    auto window = zaf::Create<zaf::Window>();

    zaf::Point window_position{ 10.25, 10.25 };
    zaf::Size content_size{ 200.25, 300.25 };
    window->SetRect(zaf::Rect{ window_position, content_size });
    window->SetContentSize(content_size);

    //Position should not changed after setting content size.
    ASSERT_EQ(window->Position(), window_position);

    //Window size should be enlarged.
    auto window_size = window->Size();
    ASSERT_GT(window_size.width, content_size.width);
    ASSERT_GT(window_size.height, content_size.height);
    
    //Even if window handle is not created yet, content size would be adjusted.
    zaf::Size adjusted_content_size;
    adjusted_content_size.width = std::floor(content_size.width);
    adjusted_content_size.height = std::floor(content_size.height);
    ASSERT_EQ(window->ContentSize(), adjusted_content_size);

    auto holder = window->CreateHandle();

    //After creating handle, content size should not changed.
    ASSERT_EQ(window->ContentSize(), adjusted_content_size);
}


TEST_F(WindowTest, RemoveFocusedControl) {

    auto control = zaf::Create<zaf::Control>();
    auto window = zaf::Create<zaf::Window>();
    window->RootControl()->AddChild(control);

    window->Show();

    control->SetCanFocus(true);
    control->SetIsFocused(true);
    ASSERT_EQ(window->FocusedControl(), control);

    //RemoveChild
    window->RootControl()->RemoveChild(control);
    ASSERT_EQ(window->FocusedControl(), nullptr);

    window->RootControl()->AddChild(control);
    control->SetIsFocused(true);
    ASSERT_EQ(window->FocusedControl(), control);

    //RemoveAllChildren
    window->RootControl()->RemoveAllChildren();
    ASSERT_EQ(window->FocusedControl(), nullptr);

    window->Destroy();
}


TEST_F(WindowTest, RemoveFocusedControlIndirectly) {

    auto focused_control = zaf::Create<zaf::Control>();
    focused_control->SetCanFocus(true);

    auto parent = zaf::Create<zaf::Control>();
    parent->AddChild(focused_control);

    auto window = zaf::Create<zaf::Window>();
    window->RootControl()->AddChild(parent);

    window->Show();

    focused_control->SetIsFocused(true);
    ASSERT_EQ(window->FocusedControl(), focused_control);

    //RemoveChild
    window->RootControl()->RemoveChild(parent);
    ASSERT_EQ(window->FocusedControl(), nullptr);

    window->RootControl()->AddChild(parent);
    focused_control->SetIsFocused(true);
    ASSERT_EQ(window->FocusedControl(), focused_control);

    //RemoveAllChildren
    window->RootControl()->RemoveAllChildren();
    ASSERT_EQ(window->FocusedControl(), nullptr);

    window->Destroy();
}


TEST_F(WindowTest, CreateHandle) {

    auto window = zaf::Create<zaf::Window>();

    HWND handle{};

    window->Disposables() += window->HandleCreatedEvent().Subscribe(
        [&handle](const zaf::HandleCreatedInfo& event_info) {

        handle = zaf::As<zaf::Window>(event_info.Source())->Handle();
    });

    ASSERT_EQ(window->Handle(), handle);
}


TEST_F(WindowTest, ShowWindowEvent) {

    bool is_show_event_called{};
    bool is_hide_event_called{};

    auto window = zaf::Create<zaf::Window>();
    window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
    window->SetRect(zaf::Rect{ 0, 0, 1, 1 });

    window->Disposables() += window->ShowEvent().Subscribe(
        [&is_show_event_called](const zaf::ShowInfo& event_info) {
        is_show_event_called = true;
    });

    window->Disposables() += window->HideEvent().Subscribe(
        [&is_hide_event_called](const zaf::HideInfo& event_info) {
        is_hide_event_called = true;
    });

    window->Show();
    ASSERT_TRUE(is_show_event_called);

    window->Hide();
    ASSERT_TRUE(is_hide_event_called);
}


TEST_F(WindowTest, FocusEvent) {

    auto window = zaf::Create<zaf::Window>();
    window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
    window->SetRect(zaf::Rect{ 0, 0, 1, 1 });
    window->Show();

    auto is_focus_lost_event_called = std::make_shared<bool>();
    auto is_focus_gained_event_called = std::make_shared<bool>();

    window->Disposables() += window->FocusLostEvent().Subscribe(
        [is_focus_lost_event_called](const zaf::WindowFocusLostInfo& event_info) {
        *is_focus_lost_event_called = true;
    });

    window->Disposables() += window->FocusGainedEvent().Subscribe(
        [is_focus_gained_event_called](const zaf::WindowFocusGainedInfo& event_info) {
        *is_focus_gained_event_called = true;
    });

    SetFocus(nullptr);
    ASSERT_TRUE(*is_focus_lost_event_called);

    SetFocus(window->Handle());
    ASSERT_TRUE(*is_focus_gained_event_called);
}


TEST_F(WindowTest, Close) {

    // Close window normally.
    {
        auto window = zaf::Create<zaf::Window>();
        auto holder = window->CreateHandle();
        window->Close();
        ASSERT_EQ(window->Handle(), nullptr);
        ASSERT_EQ(window->HandleState(), zaf::WindowHandleState::Destroyed);
    }

    // Prevent window from closing in ClosingEvent.
    {
        auto window = zaf::Create<zaf::Window>();
        auto holder = window->CreateHandle();
        auto sub = window->ClosingEvent().Subscribe([](const zaf::ClosingInfo& event_info) {
            event_info.SetCanClose(false);
        });
        window->Close();
        ASSERT_NE(window->Handle(), nullptr);
        ASSERT_EQ(window->HandleState(), zaf::WindowHandleState::Created);
    }
}


TEST_F(WindowTest, SetRootControl) {

    //Set root control with invalid argument.
    {
        auto window = zaf::Create<zaf::Window>();

        //Set nullptr.
        ASSERT_THROW(window->SetRootControl(nullptr), std::logic_error);

        //Set a new one which is the root control of another window.
        auto other_window = zaf::Create<zaf::Window>();
        ASSERT_THROW(window->SetRootControl(other_window->RootControl()), std::logic_error);

        //Set a new one which has parent.
        auto parent = zaf::Create<zaf::Control>();
        auto child = zaf::Create<zaf::Control>();
        parent->AddChild(child);
        ASSERT_THROW(window->SetRootControl(child), std::logic_error);
    }

    //The same root control can be set multiple times.
    {
        auto window = zaf::Create<zaf::Window>();

        auto root_control = window->RootControl();
        window->SetRootControl(root_control);
        ASSERT_EQ(window->RootControl(), root_control);

        window->SetRootControl(root_control);
        ASSERT_EQ(window->RootControl(), root_control);
    }

    //Replace root control.
    {
        auto window = zaf::Create<zaf::Window>();

        auto label = zaf::Create<zaf::Label>();
        window->SetRootControl(label);
        ASSERT_EQ(window->RootControl(), label);
    }
}


TEST_F(WindowTest, WhenNotSizingOrMoving) {

    //Event emits immediate if the handle is not created.
    {
        auto window = zaf::Create<zaf::Window>();
        bool on_success_called{};
        auto subscription = window->WhenNotSizingOrMoving().Subscribe([&](zaf::None) {
            on_success_called = true;
        });
        ASSERT_TRUE(on_success_called);
    }

    //Event emits immediate if the handle is created.
    {
        auto window = zaf::Create<zaf::Window>();
        auto holder = window->CreateHandle();
        bool on_success_called{};
        auto subscription = window->WhenNotSizingOrMoving().Subscribe([&](zaf::None) {
            on_success_called = true;
        });
        ASSERT_TRUE(on_success_called);
    }

    //Event doesn't emit if the window is being resized or moved.
    {
        auto window = zaf::Create<zaf::Window>();
        auto holder = window->CreateHandle();
        window->Messager().Send(WM_ENTERSIZEMOVE, 0, 0);

        int call_count{};
        auto subscription = window->WhenNotSizingOrMoving().Subscribe([&](zaf::None) {
            call_count++;
        });
        ASSERT_EQ(call_count, 0);

        //Event emits when the window finishes resizing or moving.
        window->Messager().Send(WM_EXITSIZEMOVE, 0, 0);
        ASSERT_EQ(call_count, 1);

        //Previous observable should not emit again.
        window->Messager().Send(WM_ENTERSIZEMOVE, 0, 0);
        window->Messager().Send(WM_EXITSIZEMOVE, 0, 0);
        ASSERT_EQ(call_count, 1);
    }

    //Event doesn't emit if the window is destroy during resizing or moving.
    {
        auto window = zaf::Create<zaf::Window>();
        auto holder = window->CreateHandle();
        window->Messager().Send(WM_ENTERSIZEMOVE, 0, 0);

        int call_count{};
        auto subscription = window->WhenNotSizingOrMoving().Subscribe([&](zaf::None) {
            call_count++;
        });
        window->Destroy();
        ASSERT_EQ(call_count, 0);
    }
}

}