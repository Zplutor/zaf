#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>
#include <zaf/graphic/dpi.h>
#include <zaf/window/window.h>

TEST(ControlMouseEventRoutingTest, RoutingPath) {

    auto layouter = zaf::Create<zaf::VerticalLayouter>();
    layouter->SetAxisAlignment(zaf::AxisAlignment::Center);
    layouter->SetCrossAxisAlignment(zaf::AxisAlignment::Center);

    auto window = zaf::Create<zaf::Window>();
    window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
    window->SetPosition({});
    window->SetContentSize(zaf::Size{ 200, 200 });
    window->RootControl()->SetLayouter(layouter);

    auto middle_control = zaf::Create<zaf::Control>();
    middle_control->SetFixedSize(zaf::Size{ 80, 80 });
    middle_control->SetLayouter(layouter);
    window->RootControl()->AddChild(middle_control);

    auto inner_control = zaf::Create<zaf::Control>();
    inner_control->SetFixedSize(zaf::Size{ 60, 60 });
    middle_control->AddChild(inner_control);

    zaf::SubscriptionSet subs;

    int event_seed{};

    subs += window->RootControl()->PreMouseMoveEvent().Subscribe(
        [&](const zaf::PreMouseMoveInfo& event_info) {

        ++event_seed;
        ASSERT_EQ(event_seed, 1);
        ASSERT_TRUE(event_info.Source()->IsEqual(*window->RootControl()));
        ASSERT_EQ(event_info.PositionAtSource(), zaf::Point(100, 100));
        ASSERT_TRUE(event_info.Sender()->IsEqual(*window->RootControl()));
        ASSERT_EQ(event_info.PositionAtSender(), zaf::Point(100, 100));
    });

    subs += middle_control->PreMouseMoveEvent().Subscribe(
        [&](const zaf::PreMouseMoveInfo& event_info) {

        ++event_seed;
        ASSERT_EQ(event_seed, 2);
        ASSERT_TRUE(event_info.Source()->IsEqual(*window->RootControl()));
        ASSERT_EQ(event_info.PositionAtSource(), zaf::Point(100, 100));
        ASSERT_TRUE(event_info.Sender()->IsEqual(*middle_control));
        ASSERT_EQ(event_info.PositionAtSender(), zaf::Point(40, 40));
    });

    subs += inner_control->PreMouseMoveEvent().Subscribe(
        [&](const zaf::PreMouseMoveInfo& event_info) {

        ++event_seed;
        ASSERT_EQ(event_seed, 3);
        ASSERT_TRUE(event_info.Source()->IsEqual(*window->RootControl()));
        ASSERT_EQ(event_info.PositionAtSource(), zaf::Point(100, 100));
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
    SendMessage(window->Handle(), WM_MOUSEMOVE, 0, MAKELPARAM(coordinate, coordinate));

    window->Destroy();
}