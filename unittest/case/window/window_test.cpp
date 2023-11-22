#include <gtest/gtest.h>
#include <zaf/base/as.h>
#include <zaf/control/label.h>
#include <zaf/creation.h>
#include <zaf/graphic/alignment.h>
#include <zaf/graphic/dpi.h>
#include <zaf/window/window.h>
#include <zaf/window/window_class.h>
#include <zaf/window/window_class_registry.h>

TEST(WindowTest, ClassName) {

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


TEST(WindowTest, SetRectBeforeCreate) {

    auto window = zaf::Create<zaf::Window>();
    window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);

    zaf::Rect initial_rect{ 10.25, 20.25, 400.25, 500.25 };
    window->SetRect(initial_rect);
    ASSERT_EQ(window->Rect(), initial_rect);

    auto holder = window->CreateHandle();
    //After creating the window, Rect() should use the actual window rect.
    auto pixel_rect = zaf::Align(zaf::FromDIPs(initial_rect, window->GetDPI()));
    auto rect_after_create = zaf::ToDIPs(pixel_rect, window->GetDPI());
    ASSERT_EQ(window->Rect(), rect_after_create);

    window->Destroy();
    //The rect should be reserved after window is destroyed.
    ASSERT_EQ(window->Rect(), rect_after_create);
}


TEST(WindowTest, SetRectAfterCreate) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();

    zaf::Rect rect{ 10.25, 20.25, 400.25, 500.25 };
    window->SetRect(rect);

    //After calling SetRect(), the rect should be adjusted to the actual window rect.
    auto pixel_rect = zaf::Align(zaf::FromDIPs(rect, window->GetDPI()));
    auto rect_after_set = zaf::ToDIPs(pixel_rect, window->GetDPI());
    ASSERT_EQ(window->Rect(), rect_after_set);

    window->Destroy();
    //The rect should be reserved after window is destroyed.
    ASSERT_EQ(window->Rect(), rect_after_set);
}


TEST(WindowTest, GetRectAfterChangeRect) {

    auto window = zaf::Create<zaf::Window>();
    window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
    window->SetRect(zaf::Rect{ 100, 100, 400, 400 });

    //Simulate user resize the window.
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

    //Simulate user move the window.
    holder = window->CreateHandle();
    SetWindowPos(
        window->Handle(),
        nullptr,
        static_cast<int>(zaf::FromDIPs(200, window->GetDPI())),
        static_cast<int>(zaf::FromDIPs(300, window->GetDPI())),
        0,
        0,
        SWP_NOSIZE | SWP_NOACTIVATE);
    window->Destroy();

    new_rect = zaf::Rect{ 200, 300, 400, 500 };
    ASSERT_EQ(window->Rect(), new_rect);
}


TEST(WindowTest, SetContentSize) {

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


TEST(WindowTest, RemoveFocusedControl) {

    auto control = zaf::Create<zaf::Control>();
    auto window = zaf::Create<zaf::Window>();
    window->RootControl()->AddChild(control);

    control->SetCanFocused(true);
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
}


TEST(WindowTest, RemoveFocusedControlIndirectly) {

    auto focused_control = zaf::Create<zaf::Control>();
    focused_control->SetCanFocused(true);

    auto parent = zaf::Create<zaf::Control>();
    parent->AddChild(focused_control);

    auto window = zaf::Create<zaf::Window>();
    window->RootControl()->AddChild(parent);

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
}


TEST(WindowTest, CreateHandle) {

    auto window = zaf::Create<zaf::Window>();

    HWND handle{};

    window->Subscriptions() += window->HandleCreatedEvent().Subscribe(
        [&handle](const zaf::HandleCreatedInfo& event_info) {

        handle = zaf::As<zaf::Window>(event_info.Source())->Handle();
    });

    ASSERT_EQ(window->Handle(), handle);
}


TEST(WindowTest, ShowWindowEvent) {

    bool is_show_event_called{};
    bool is_hide_event_called{};

    auto window = zaf::Create<zaf::Window>();
    window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
    window->SetRect(zaf::Rect{ 0, 0, 1, 1 });

    window->Subscriptions() += window->ShowEvent().Subscribe(
        [&is_show_event_called](const zaf::ShowInfo& event_info) {
        is_show_event_called = true;
    });

    window->Subscriptions() += window->HideEvent().Subscribe(
        [&is_hide_event_called](const zaf::HideInfo& event_info) {
        is_hide_event_called = true;
    });

    window->Show();
    ASSERT_TRUE(is_show_event_called);

    window->Hide();
    ASSERT_TRUE(is_hide_event_called);
}


TEST(WindowTest, FocusEvent) {

    auto window = zaf::Create<zaf::Window>();
    window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
    window->SetRect(zaf::Rect{ 0, 0, 1, 1 });
    window->Show();

    auto is_focus_lost_event_called = std::make_shared<bool>();
    auto is_focus_gained_event_called = std::make_shared<bool>();

    window->Subscriptions() += window->FocusLostEvent().Subscribe(
        [is_focus_lost_event_called](const zaf::WindowFocusLostInfo& event_info) {
        *is_focus_lost_event_called = true;
    });

    window->Subscriptions() += window->FocusGainedEvent().Subscribe(
        [is_focus_gained_event_called](const zaf::WindowFocusGainedInfo& event_info) {
        *is_focus_gained_event_called = true;
    });

    SetFocus(nullptr);
    ASSERT_TRUE(*is_focus_lost_event_called);

    SetFocus(window->Handle());
    ASSERT_TRUE(*is_focus_gained_event_called);
}


TEST(WindowTest, Close) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    window->Close();
    ASSERT_EQ(window->Handle(), nullptr);

    holder = window->CreateHandle();
    window->Subscriptions() += window->ClosingEvent().Subscribe(
        [](const zaf::ClosingInfo& event_info) {

        event_info.SetCanClose(false);
    });
    window->Close();
    ASSERT_NE(window->Handle(), nullptr);
}


TEST(WindowTest, SetRootControl) {

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


TEST(WindowTest, WhenNotSizingOrMoving) {

    auto window = zaf::Create<zaf::Window>();
    
    //Observable emits immediate if the handle is not created.
    {
        bool on_next_called{};
        bool on_completed_called{};
        auto subscription = window->WhenNotSizingOrMoving().Subscribe([&](zaf::None) {
            on_next_called = true;
        },
        [&]() {
            on_completed_called = true;
        });
        ASSERT_TRUE(on_next_called);
        ASSERT_TRUE(on_completed_called);
    }

    //Observable emits immediate if the handle is created.
    {
        auto holder = window->CreateHandle();
        bool on_next_called{};
        bool on_completed_called{};
        auto subscription = window->WhenNotSizingOrMoving().Subscribe([&](zaf::None) {
            on_next_called = true;
        },
        [&]() {
            on_completed_called = true;
        });
        ASSERT_TRUE(on_next_called);
        ASSERT_TRUE(on_completed_called);
    }

    {
        //Observable doesn't emit if the window is being resized or moved.
        auto holder = window->CreateHandle();
        window->Messager().Send(WM_ENTERSIZEMOVE, 0, 0);

        int call_count{};
        bool is_completed_called{};
        auto subscription = window->WhenNotSizingOrMoving().Subscribe([&](zaf::None) {
            call_count++;
        },
        [&]() {
            is_completed_called = true;
        });
        ASSERT_EQ(call_count, 0);
        ASSERT_FALSE(is_completed_called);

        //Observable emits when the window finishes resizing or moving.
        window->Messager().Send(WM_EXITSIZEMOVE, 0, 0);
        ASSERT_EQ(call_count, 1);
        ASSERT_TRUE(is_completed_called);

        //Previous observable should not emit again.
        window->Messager().Send(WM_ENTERSIZEMOVE, 0, 0);
        window->Messager().Send(WM_EXITSIZEMOVE, 0, 0);
        ASSERT_EQ(call_count, 1);
    }

    {
        //Observable doesn't emit if the window is destroy during resizing or moving.
        auto holder = window->CreateHandle();
        window->Messager().Send(WM_ENTERSIZEMOVE, 0, 0);

        int call_count{};
        auto subscription = window->WhenNotSizingOrMoving().Subscribe([&](zaf::None) {
            call_count++;
        });
        window->Destroy();
        ASSERT_EQ(call_count, 0);

        holder = window->CreateHandle();
        //Previous observable should not emit again.
        window->Messager().Send(WM_ENTERSIZEMOVE, 0, 0);
        window->Messager().Send(WM_EXITSIZEMOVE, 0, 0);
        ASSERT_EQ(call_count, 0);
    }
}