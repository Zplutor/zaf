#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

TEST(ControlKeyboardEventRoutingTest, RoutingPath) {

    auto window = zaf::Create<zaf::Window>();
    window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
    window->SetPosition({});
    window->SetSize(zaf::Size(200, 200));

    auto parent = zaf::Create<zaf::Control>();
    parent->SetCanFocused(true);
    window->RootControl()->AddChild(parent);

    auto focused = zaf::Create<zaf::Control>();
    focused->SetCanFocused(true);
    parent->AddChild(focused);

    auto not_focused = zaf::Create<zaf::Control>();
    not_focused->SetCanFocused(true);
    focused->AddChild(not_focused);

    zaf::SubscriptionSet subs;
    int event_seed{};

    subs += window->RootControl()->PreKeyDownEvent().Subscribe(
        [&](const zaf::PreKeyDownInfo& event_info) {

        ++event_seed;
        ASSERT_EQ(event_seed, 1);
        ASSERT_EQ(event_info.Source(), window->RootControl());
        ASSERT_EQ(event_info.Sender(), window->RootControl());
    });

    subs += parent->PreKeyDownEvent().Subscribe([&](const zaf::PreKeyDownInfo& event_info) {
        ++event_seed;
        ASSERT_EQ(event_seed, 2);
        ASSERT_EQ(event_info.Source(), window->RootControl());
        ASSERT_EQ(event_info.Sender(), parent);
    });

    subs += focused->PreKeyDownEvent().Subscribe([&](const zaf::PreKeyDownInfo& event_info) {
        ++event_seed;
        ASSERT_EQ(event_seed, 3);
        ASSERT_EQ(event_info.Source(), window->RootControl());
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