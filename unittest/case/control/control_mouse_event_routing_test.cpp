#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>
#include <zaf/graphic/dpi.h>
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

TEST(ControlMouseEventRoutingTest, RoutingPath) {

    auto layouter = zaf::Create<zaf::VerticalLayouter>();
    layouter->SetAxisAlignment(zaf::AxisAlignment::Center);
    layouter->SetCrossAxisAlignment(zaf::AxisAlignment::Center);

    auto window = CreateTestWindow();
    window->RootControl()->SetLayouter(layouter);

    auto middle_control = zaf::Create<zaf::Control>();
    middle_control->SetFixedSize(zaf::Size{ 80, 80 });
    middle_control->SetLayouter(layouter);
    window->RootControl()->AddChild(middle_control);

    auto inner_control = zaf::Create<zaf::Control>();
    inner_control->SetFixedSize(zaf::Size{ 60, 60 });
    middle_control->AddChild(inner_control);

    zaf::rx::SubscriptionBag subs;

    int event_seed{};

    subs += window->RootControl()->PreMouseMoveEvent().Subscribe(
        [&](const zaf::PreMouseMoveInfo& event_info) {

        ++event_seed;
        ASSERT_EQ(event_seed, 1);
        ASSERT_TRUE(event_info.Source()->IsEqual(*inner_control));
        ASSERT_EQ(event_info.PositionAtSource(), zaf::Point(30, 30));
        ASSERT_TRUE(event_info.Sender()->IsEqual(*window->RootControl()));
        ASSERT_EQ(event_info.PositionAtSender(), zaf::Point(100, 100));
    });

    subs += middle_control->PreMouseMoveEvent().Subscribe(
        [&](const zaf::PreMouseMoveInfo& event_info) {

        ++event_seed;
        ASSERT_EQ(event_seed, 2);
        ASSERT_TRUE(event_info.Source()->IsEqual(*inner_control));
        ASSERT_EQ(event_info.PositionAtSource(), zaf::Point(30, 30));
        ASSERT_TRUE(event_info.Sender()->IsEqual(*middle_control));
        ASSERT_EQ(event_info.PositionAtSender(), zaf::Point(40, 40));
    });

    subs += inner_control->PreMouseMoveEvent().Subscribe(
        [&](const zaf::PreMouseMoveInfo& event_info) {

        ++event_seed;
        ASSERT_EQ(event_seed, 3);
        ASSERT_TRUE(event_info.Source()->IsEqual(*inner_control));
        ASSERT_EQ(event_info.PositionAtSource(), zaf::Point(30, 30));
        ASSERT_TRUE(event_info.Sender()->IsEqual(*inner_control));
        ASSERT_EQ(event_info.PositionAtSender(), zaf::Point(30, 30));
    });

    subs += inner_control->MouseMoveEvent().Subscribe(
        [&](const zaf::MouseMoveInfo& event_info) {

        ++event_seed;
        ASSERT_EQ(event_seed, 4);
        ASSERT_TRUE(event_info.Source()->IsEqual(*inner_control));
        ASSERT_EQ(event_info.PositionAtSource(), zaf::Point(30, 30));
        ASSERT_TRUE(event_info.Sender()->IsEqual(*inner_control));
        ASSERT_EQ(event_info.PositionAtSender(), zaf::Point(30, 30));
    });

    subs += middle_control->MouseMoveEvent().Subscribe(
        [&](const zaf::MouseMoveInfo& event_info) {

        ++event_seed;
        ASSERT_EQ(event_seed, 5);
        ASSERT_TRUE(event_info.Source()->IsEqual(*inner_control));
        ASSERT_EQ(event_info.PositionAtSource(), zaf::Point(30, 30));
        ASSERT_TRUE(event_info.Sender()->IsEqual(*middle_control));
        ASSERT_EQ(event_info.PositionAtSender(), zaf::Point(40, 40));
    });

    subs += window->RootControl()->MouseMoveEvent().Subscribe(
        [&](const zaf::MouseMoveInfo& event_info) {
    
        ++event_seed;
        ASSERT_EQ(event_seed, 6);
        ASSERT_TRUE(event_info.Source()->IsEqual(*inner_control));
        ASSERT_EQ(event_info.PositionAtSource(), zaf::Point(30, 30));
        ASSERT_TRUE(event_info.Sender()->IsEqual(*window->RootControl()));
        ASSERT_EQ(event_info.PositionAtSender(), zaf::Point(100, 100));
    });
    
    window->Show();

    auto coordinate = zaf::FromDIPs(100, window->GetDPI());
    window->Messager().Send(WM_MOUSEMOVE, 0, MAKELPARAM(coordinate, coordinate));

    //Make sure events get called.
    ASSERT_NE(event_seed, 0);

    window->Destroy();
}


TEST(ControlMouseEventRoutingTest, IsHandled) {

    auto window = CreateTestWindow();

    zaf::rx::SubscriptionBag subs;
    subs += window->RootControl()->PreMouseMoveEvent().Subscribe(
        [](const zaf::PreMouseMoveInfo& event_info) {
    
        event_info.MarkAsHandled();
    });

    bool is_handled{};
    subs += window->RootControl()->MouseMoveEvent().Subscribe(
        [&](const zaf::MouseMoveInfo& event_info) {

        is_handled = event_info.IsHandled();
    });

    window->Show();

    auto coordinate = zaf::FromDIPs(100, window->GetDPI());
    window->Messager().Send(WM_MOUSEMOVE, 0, MAKELPARAM(coordinate, coordinate));

    ASSERT_TRUE(is_handled);

    window->Destroy();
}


TEST(ControlMouseEventRoutingTest, EventType) {

    auto window = CreateTestWindow();

    UINT pre_event_message_id{};
    UINT event_message_id{};

    auto pre_event_handler = [&](const auto& event_info) {
        pre_event_message_id = event_info.Message().ID();
    };

    auto event_handler = [&](const auto& event_info) {
        event_message_id = event_info.Message().ID();
    };

    zaf::rx::SubscriptionBag subs;
    subs += window->RootControl()->PreMouseMoveEvent().Subscribe(pre_event_handler);
    subs += window->RootControl()->MouseMoveEvent().Subscribe(event_handler);
    subs += window->RootControl()->PreMouseDownEvent().Subscribe(pre_event_handler);
    subs += window->RootControl()->MouseDownEvent().Subscribe(event_handler);
    subs += window->RootControl()->PreMouseUpEvent().Subscribe(pre_event_handler);
    subs += window->RootControl()->MouseUpEvent().Subscribe(event_handler);
    subs += window->RootControl()->PreMouseWheelEvent().Subscribe(pre_event_handler);
    subs += window->RootControl()->MouseWheelEvent().Subscribe(event_handler);

    window->Show();

    auto coordinate = zaf::FromDIPs(100, window->GetDPI());
    auto client_messages = { 
        WM_MOUSEMOVE,
        WM_LBUTTONDOWN,
        WM_MBUTTONDOWN,
        WM_RBUTTONDOWN,
        WM_LBUTTONUP,
        WM_MBUTTONUP,
        WM_RBUTTONUP,
    };
    for (auto each_message : client_messages) {

        pre_event_message_id = 0;
        event_message_id = 0;

        window->Messager().Send(each_message, 0, MAKELPARAM(coordinate, coordinate));

        ASSERT_EQ(pre_event_message_id, each_message);
        ASSERT_EQ(event_message_id, each_message);
    }

    zaf::Point position{ 100, 100 };
    position = window->TranslateToScreen(position);
    auto screen_position = zaf::FromDIPs(position, window->GetDPI());
    auto screen_messages = {
        WM_MOUSEWHEEL,
        WM_MOUSEHWHEEL,
    };
    for (auto each_message : screen_messages) {

        pre_event_message_id = 0;
        event_message_id = 0;

        window->Messager().Send(
            each_message, 
            0, 
            MAKELPARAM(screen_position.x, screen_position.y));

        ASSERT_EQ(pre_event_message_id, each_message);
        ASSERT_EQ(event_message_id, each_message);
    }

    window->Destroy();
}

