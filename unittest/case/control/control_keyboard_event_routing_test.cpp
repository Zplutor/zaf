#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

namespace {

std::shared_ptr<zaf::Window> CreateTestWindow() {

    auto window = zaf::Create<zaf::Window>();
    window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
    window->SetPosition({});
    window->SetContentSize(zaf::Size{ 200, 200 });
    return window;
}

}

TEST(ControlKeyboardEventRoutingTest, RoutingPath) {

    auto window = CreateTestWindow();

    auto parent = zaf::Create<zaf::Control>();
    parent->SetCanFocus(true);
    window->RootControl()->AddChild(parent);

    auto focused = zaf::Create<zaf::Control>();
    focused->SetCanFocus(true);
    parent->AddChild(focused);

    auto not_focused = zaf::Create<zaf::Control>();
    not_focused->SetCanFocus(true);
    focused->AddChild(not_focused);

    zaf::rx::DisposeBag subs;
    int event_seed{};

    subs += window->RootControl()->PreKeyDownEvent().Subscribe(
        [&](const zaf::PreKeyDownInfo& event_info) {

        ++event_seed;
        ASSERT_EQ(event_seed, 1);
        ASSERT_EQ(event_info.Source(), focused);
        ASSERT_EQ(event_info.Sender(), window->RootControl());
    });

    subs += parent->PreKeyDownEvent().Subscribe([&](const zaf::PreKeyDownInfo& event_info) {
        ++event_seed;
        ASSERT_EQ(event_seed, 2);
        ASSERT_EQ(event_info.Source(), focused);
        ASSERT_EQ(event_info.Sender(), parent);
    });

    subs += focused->PreKeyDownEvent().Subscribe([&](const zaf::PreKeyDownInfo& event_info) {
        ++event_seed;
        ASSERT_EQ(event_seed, 3);
        ASSERT_EQ(event_info.Source(), focused);
        ASSERT_EQ(event_info.Sender(), focused);
    });

    subs += not_focused->PreKeyDownEvent().Subscribe([&](const zaf::PreKeyDownInfo& event_info) {
        GTEST_FAIL();
    });

    subs += not_focused->KeyDownEvent().Subscribe([&](const zaf::KeyDownInfo& event_info) {
        GTEST_FAIL();
    });

    subs += focused->KeyDownEvent().Subscribe([&](const zaf::KeyDownInfo& event_info) {
        ++event_seed;
        ASSERT_EQ(event_seed, 4);
        ASSERT_EQ(event_info.Source(), focused);
        ASSERT_EQ(event_info.Sender(), focused);
    });

    subs += parent->KeyDownEvent().Subscribe([&](const zaf::KeyDownInfo& event_info) {
        ++event_seed;
        ASSERT_EQ(event_seed, 5);
        ASSERT_EQ(event_info.Source(), focused);
        ASSERT_EQ(event_info.Sender(), parent);
    });

    subs += window->RootControl()->KeyDownEvent().Subscribe(
        [&](const zaf::KeyDownInfo& event_info) {

        ++event_seed;
        ASSERT_EQ(event_seed, 6);
        ASSERT_EQ(event_info.Source(), focused);
        ASSERT_EQ(event_info.Sender(), window->RootControl());
    });

    window->Show();

    focused->SetIsFocused(true);
    SendMessage(window->Handle(), WM_KEYDOWN, VK_RETURN, 0);

    //Make sure events get called.
    ASSERT_NE(event_seed, 0);

    window->Destroy();
}



TEST(ControlKeyboardEventRoutingTest, IsHandled) {

    auto window = CreateTestWindow();
    window->RootControl()->SetCanFocus(true);

    zaf::rx::DisposeBag subs;
    subs += window->RootControl()->PreKeyDownEvent().Subscribe(
        [](const zaf::PreKeyDownInfo& event_info) {

        event_info.MarkAsHandled();
    });

    bool is_handled{};
    subs += window->RootControl()->KeyDownEvent().Subscribe(
        [&](const zaf::KeyDownInfo& event_info) {

        is_handled = event_info.IsHandled();
    });

    window->Show();

    window->RootControl()->SetIsFocused(true);
    SendMessage(window->Handle(), WM_KEYDOWN, VK_RETURN, 0);

    ASSERT_TRUE(is_handled);

    window->Destroy();
}


TEST(ControlKeyboardEventRoutingTest, EventType) {

    auto window = CreateTestWindow();
    window->RootControl()->SetCanFocus(true);

    UINT pre_event_message_id{};
    UINT event_message_id{};
    auto pre_event_handler = [&](const auto& event_info) {
        pre_event_message_id = event_info.Message().ID();
    };
    auto event_handler = [&](const auto& event_info) {
        event_message_id = event_info.Message().ID();
    };

    zaf::rx::DisposeBag subs;

    struct {
        UINT message_id;
        std::function<void()> event_register;
    } test_items[] = {
        { 
            WM_KEYDOWN, 
            [&]() { 
                subs += window->RootControl()->PreKeyDownEvent().Subscribe(pre_event_handler);
                subs += window->RootControl()->KeyDownEvent().Subscribe(event_handler);
            }
        },
        {
            WM_KEYUP,
            [&]() {
                subs += window->RootControl()->PreKeyUpEvent().Subscribe(pre_event_handler);
                subs += window->RootControl()->KeyUpEvent().Subscribe(event_handler);
            }
        },
        {
            WM_CHAR,
            [&]() {
                subs += window->RootControl()->PreCharInputEvent().Subscribe(pre_event_handler);
                subs += window->RootControl()->CharInputEvent().Subscribe(event_handler);
            }
        },
        {
            WM_SYSKEYDOWN,
            [&]() {
                subs += window->RootControl()->PreSysKeyDownEvent().Subscribe(pre_event_handler);
                subs += window->RootControl()->SysKeyDownEvent().Subscribe(event_handler);
            }
        },
        {
            WM_SYSKEYUP,
            [&]() {
                subs += window->RootControl()->PreSysKeyUpEvent().Subscribe(pre_event_handler);
                subs += window->RootControl()->SysKeyUpEvent().Subscribe(event_handler);
            }
        },
        {
            WM_SYSCHAR,
            [&]() {
                subs += window->RootControl()->PreSysCharInputEvent().Subscribe(pre_event_handler);
                subs += window->RootControl()->SysCharInputEvent().Subscribe(event_handler);
            }
        }
    };


    window->Show();
    window->RootControl()->SetIsFocused(true);

    for (const auto& each_item : test_items) {

        pre_event_message_id = 0;
        event_message_id = 0;
        subs.Clear();

        each_item.event_register();

        SendMessage(window->Handle(), each_item.message_id, VK_RETURN, 0);

        ASSERT_EQ(pre_event_message_id, each_item.message_id);
        ASSERT_EQ(event_message_id, each_item.message_id);
    }

    window->Destroy();
}