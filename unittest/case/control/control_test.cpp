#include <gtest/gtest.h>
#include <zaf/application.h>
#include <zaf/base/as.h>
#include <zaf/control/control.h>
#include <zaf/control/label.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/control/linear_box.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

TEST(ControlTest, GeometryInWindow) {

    //The control doesn't have parent.
    {
        auto control = zaf::Create<zaf::Control>();
        ASSERT_FALSE(control->PositionInWindow().has_value());
        ASSERT_FALSE(control->RectInWindow().has_value());
        ASSERT_FALSE(control->ContentRectInWindow().has_value());
    }

    //The control is the root control of a window.
    {
        auto window = zaf::Create<zaf::Window>();
        window->SetContentSize(zaf::Size{ 300, 300 });
        auto holder = window->CreateHandle();

        const auto& root_control = window->RootControl();
        root_control->SetBorder(zaf::Frame{ 1 });
        root_control->SetPadding(zaf::Frame{ 2 });

        ASSERT_EQ(root_control->PositionInWindow(), zaf::Point());
        ASSERT_EQ(root_control->RectInWindow(), zaf::Rect(zaf::Point(), zaf::Size(300, 300)));
        ASSERT_EQ(
            root_control->ContentRectInWindow(),
            zaf::Rect(zaf::Point(3, 3), zaf::Size(294, 294)));
    }

    //The control belongs to a parent which is not in a window.
    {
        auto control = zaf::Create<zaf::Control>();
        auto parent = zaf::Create<zaf::Control>();
        parent->AddChild(control);

        ASSERT_FALSE(control->PositionInWindow().has_value());
        ASSERT_FALSE(control->RectInWindow().has_value());
        ASSERT_FALSE(control->ContentRectInWindow().has_value());
    }

    //The control belongs to a parent which is in a window.
    {
        auto control = zaf::Create<zaf::Control>();
        control->SetRect(zaf::Rect{ 11, 12, 90, 80 });
        control->SetPadding(zaf::Frame{ 2 });

        auto parent = zaf::Create<zaf::Control>();
        parent->SetBorder(zaf::Frame{ 5, 6, 0, 0 });
        parent->SetPadding(zaf::Frame{ 20, 30, 0, 0 });
        parent->AddChild(control);

        auto window = zaf::Create<zaf::Window>();
        window->RootControl()->AddChild(parent);
        auto holder = window->CreateHandle();

        ASSERT_EQ(control->PositionInWindow(), zaf::Point(36, 48));
        ASSERT_EQ(control->RectInWindow(), zaf::Rect(zaf::Point(36, 48), zaf::Size(90, 80)));
        ASSERT_EQ(
            control->ContentRectInWindow(),
            zaf::Rect(zaf::Point(38, 50), zaf::Size(86, 76)));
    }
}


TEST(ControlTest, RectInSelf) {

    auto control = zaf::Create<zaf::Control>();
    ASSERT_EQ(control->RectInSelf(), zaf::Rect());

    control->SetRect(zaf::Rect{ 10, 10, 100, 100 });
    ASSERT_EQ(control->RectInSelf(), zaf::Rect(zaf::Point(), zaf::Size(100, 100)));
}


TEST(ControlTest, ContentRect) {

    auto control = zaf::Create<zaf::Control>();
    ASSERT_EQ(control->ContentRect(), zaf::Rect());

    //Size is empty, and there are border and padding.
    control->SetBorder(zaf::Frame{ 10, 20, 30, 40 });
    control->SetPadding(zaf::Frame{ 20, 30, 40, 50 });
    ASSERT_EQ(control->ContentRect(), zaf::Rect(30, 50, 0, 0));

    //Size is too small to contain border and padding.
    control->SetSize(zaf::Size{ 20, 20 });
    ASSERT_EQ(control->ContentRect(), zaf::Rect(30, 50, 0, 0));

    control->SetSize(zaf::Size{ 40, 40 });
    ASSERT_EQ(control->ContentRect(), zaf::Rect(30, 50, 0, 0));

    //Size is large enough to contain border and padding.
    control->SetSize(zaf::Size{ 200, 200 });
    ASSERT_EQ(control->ContentRect(), zaf::Rect(30, 50, 100, 60));
}


TEST(ControlTest, Update) {

    auto parent = zaf::Create<zaf::Control>();
    parent->SetLayouter(zaf::Create<zaf::VerticalLayouter>());
    parent->SetSize(zaf::Size{ 100, 300 });

    auto update_guard = parent->BeginUpdate();

    auto child1 = zaf::Create<zaf::Control>();
    auto child2 = zaf::Create<zaf::Control>();
    auto child3 = zaf::Create<zaf::Control>();
    parent->AddChildren({ child1, child2, child3 });

    ASSERT_EQ(child1->Size(), zaf::Size());
    ASSERT_EQ(child2->Size(), zaf::Size());
    ASSERT_EQ(child3->Size(), zaf::Size());

    {
        auto discard_guard = std::move(update_guard);
    }

    ASSERT_EQ(child1->Size(), zaf::Size(100, 100));
    ASSERT_EQ(child2->Size(), zaf::Size(100, 100));
    ASSERT_EQ(child3->Size(), zaf::Size(100, 100));
}


TEST(ControlTest, CalculatePreferredSize) {

    auto parent = zaf::Create<zaf::Control>();

    auto child1 = zaf::Create<zaf::Control>();
    auto child2 = zaf::Create<zaf::Control>();
    parent->AddChildren({ child1, child2 });

    //Children have intersected rect.
    child1->SetRect(zaf::Rect(0, 0, 10, 10));
    child2->SetRect(zaf::Rect(5, 5, 10, 10));
    ASSERT_EQ(parent->CalculatePreferredSize(), zaf::Size(15, 15));

    //Chilren are attach to each other.
    child1->SetRect(zaf::Rect(0, 0, 10, 10));
    child2->SetRect(zaf::Rect(0, 10, 10, 10));
    ASSERT_EQ(parent->CalculatePreferredSize(), zaf::Size(10, 20));

    //Children don't have intersected rect, and are not attach to each other.
    child1->SetRect(zaf::Rect(0, 0, 10, 10));
    child2->SetRect(zaf::Rect(50, 50, 5, 5));
    ASSERT_EQ(parent->CalculatePreferredSize(), zaf::Size(55, 55));

    //Children have negative position.
    child1->SetRect(zaf::Rect(-5, -5, 10, 10));
    child2->SetRect(zaf::Rect(-30, -30, 5, 5));
    ASSERT_EQ(parent->CalculatePreferredSize(), zaf::Size(5, 5));

    //Children have margins.
    child1->SetRect(zaf::Rect(0, 0, 5, 5));
    child1->SetMargin(zaf::Frame(2));
    child2->SetRect(zaf::Rect(0, 5, 5, 5));
    child2->SetMargin(zaf::Frame(1));
    ASSERT_EQ(parent->CalculatePreferredSize(), zaf::Size(7, 11));

    //Parent has border and padding.
    parent->SetBorder(zaf::Frame(3));
    parent->SetPadding(zaf::Frame(4));
    ASSERT_EQ(parent->CalculatePreferredSize(), zaf::Size(21, 25));

    //Parent is invisible, while children is visible.
    parent->SetIsVisible(false);
    ASSERT_EQ(parent->CalculatePreferredSize(), zaf::Size(21, 25));
}


TEST(ControlTest, SetFixedWidth) {

    auto control = zaf::Create<zaf::Control>();
    control->SetSize(zaf::Size{ 100, 100 });

    std::size_t event_count{};
    auto sub = control->SizeChangedEvent().Subscribe(std::bind([&event_count]() {
        ++event_count;
    }));

    control->SetFixedWidth(33);
    //SizeChanged event should be raised.
    ASSERT_EQ(event_count, 1);
    ASSERT_EQ(control->Size(), zaf::Size(33, 100));
    ASSERT_EQ(control->MinWidth(), 33);
    ASSERT_EQ(control->MaxWidth(), 33);

    //Not allow to change the width to other value.
    control->SetWidth(40);
    ASSERT_EQ(control->Size(), zaf::Size(33, 100));
}


TEST(ControlTest, SetFixedHeight) {

    auto control = zaf::Create<zaf::Control>();
    control->SetSize(zaf::Size{ 100, 100 });

    std::size_t event_count{};
    auto sub = control->SizeChangedEvent().Subscribe(std::bind([&event_count]() {
        ++event_count;
    }));

    control->SetFixedHeight(66);
    //SizeChanged event should be raised.
    ASSERT_EQ(event_count, 1);
    ASSERT_EQ(control->Size(), zaf::Size(100, 66));
    ASSERT_EQ(control->MinHeight(), 66);
    ASSERT_EQ(control->MaxHeight(), 66);

    //Not allow to change the hieght to other value.
    control->SetHeight(40);
    ASSERT_EQ(control->Size(), zaf::Size(100, 66));
}


TEST(ControlTest, SetFixedSize) {

    auto control = zaf::Create<zaf::Control>();
    control->SetSize(zaf::Size{ 100, 100 });

    std::size_t event_count{};
    auto sub = control->SizeChangedEvent().Subscribe(std::bind([&event_count]() {
        ++event_count;
    }));

    control->SetFixedSize(zaf::Size{ 50, 60 });
    //The event should be raised once only.
    ASSERT_EQ(event_count, 1);
    ASSERT_EQ(control->Size(), zaf::Size(50, 60));
    ASSERT_EQ(control->MinWidth(), 50);
    ASSERT_EQ(control->MaxWidth(), 50);
    ASSERT_EQ(control->MinHeight(), 60);
    ASSERT_EQ(control->MaxHeight(), 60);
}


TEST(ControlTest, AutoSizeOnChildRectChanged) {

    auto parent = zaf::Create<zaf::VerticalBox>();
    parent->SetAutoSize(true);

    auto child = zaf::Create<zaf::Control>();
    parent->AddChild(child);

    child->SetFixedSize(zaf::Size{ 100, 100 });
    ASSERT_EQ(parent->Size(), zaf::Size(100, 100));
}


TEST(ControlTest, AutoSizeAndRelayoutOnChildRectChanged) {

    auto parent = zaf::Create<zaf::VerticalBox>();
    parent->SetFixedWidth(300);
    parent->SetAutoHeight(true);

    auto child1 = zaf::Create<zaf::Control>();
    child1->SetFixedHeight(100);
    parent->AddChild(child1);

    auto child2 = zaf::Create<zaf::Control>();
    child2->SetFixedHeight(10);
    parent->AddChild(child2);

    ASSERT_EQ(child1->Rect(), zaf::Rect(0, 0, 300, 100 ));
    ASSERT_EQ(child2->Rect(), zaf::Rect(0, 100, 300, 10));
    ASSERT_EQ(parent->Size(), zaf::Size(300, 110));

    child1->SetFixedHeight(10);

    ASSERT_EQ(child1->Rect(), zaf::Rect(0, 0, 300, 10));
    ASSERT_EQ(child2->Rect(), zaf::Rect(0, 10, 300, 10));
    ASSERT_EQ(parent->Size(), zaf::Size(300, 20));
}


TEST(ControlTest, AutoSize) {

    auto child = zaf::Create<zaf::Control>();
    child->SetSize(zaf::Size{ 40, 60 });

    auto parent = zaf::Create<zaf::Control>();
    parent->SetAutoSize(true);

    //Children changed.
    parent->AddChild(child);
    ASSERT_EQ(parent->Size(), child->Size());

    //Children rect changed.
    child->SetRect(zaf::Rect{ 10, 10, 50, 50 });
    ASSERT_EQ(parent->Size(), zaf::Size(60, 60));

    //Children margin changed.
    child->SetMargin(zaf::Frame{ 2, 2, 2, 2 });
    ASSERT_EQ(parent->Size(), zaf::Size(62, 62));

    //Children visiblity chagned.
    child->SetIsVisible(false);
    ASSERT_EQ(parent->Size(), zaf::Size(0, 0));
}


TEST(ControlTest, TranslateToParent) {

    auto control = zaf::Create<zaf::Control>();
    control->SetRect(zaf::Rect{ 10, 11, 100, 110 });

    //No parent
    ASSERT_THROW(control->TranslateToParent(zaf::Point{ 20, 21 }), std::logic_error);

    //Has Parent
    auto parent = zaf::Create<zaf::Control>();
    parent->SetBorder(zaf::Frame{ 1 });
    parent->SetPadding(zaf::Frame{ 2 });
    parent->AddChild(control);

    auto result = control->TranslateToParent(zaf::Point{ 20, 21 });
    ASSERT_EQ(result, zaf::Point(33, 35));
}


TEST(ControlTest, TranslateFromParent) {

    auto control = zaf::Create<zaf::Control>();
    control->SetRect(zaf::Rect{ 10, 11, 100, 110 });

    //No parent
    ASSERT_THROW(control->TranslateFromParent(zaf::Point{ 20, 21 }), std::logic_error);

    //Has parent
    auto parent = zaf::Create<zaf::Control>();
    parent->SetBorder(zaf::Frame{ 1 });
    parent->SetPadding(zaf::Frame{ 2 });
    parent->AddChild(control);

    auto result = control->TranslateFromParent(zaf::Point{ 20, 21 });
    ASSERT_EQ(result, zaf::Point(7, 7));
}


TEST(ControlTest, WindowChangedEvent) {

    struct EventInfo {
        bool is_raised{};
        std::shared_ptr<zaf::Window> previous_window;
        std::shared_ptr<zaf::Window> new_window;
    };

    auto subscribe_event = [](const std::shared_ptr<zaf::Control>& control) {

        auto result = std::make_shared<EventInfo>();
        zaf::Application::Instance().Subscriptions() += control->WindowChangedEvent().Subscribe(
            [result](const zaf::WindowChangedInfo& event_info) {

            result->is_raised = true;
            result->previous_window = event_info.PreviousWindow();
            result->new_window = zaf::As<zaf::Control>(event_info.Source())->Window();
        });
        return result;
    };

    //Add/Remove a child to parent which is not in a window, should not raise the event.
    {
        auto parent = zaf::Create<zaf::Control>();

        auto child = zaf::Create<zaf::Control>();
        auto child_event_info = subscribe_event(child);

        parent->AddChild(child);
        ASSERT_FALSE(child_event_info->is_raised);

        //Remove a child from parent which is not in a window, should not raise the event.
        parent->RemoveChild(child);
        ASSERT_FALSE(child_event_info->is_raised);
    }

    //Add parent to a window, both parent and child should raise the event.
    {
        auto parent = zaf::Create<zaf::Control>();
        auto child = zaf::Create<zaf::Control>();
        parent->AddChild(child);

        auto child_event_info = subscribe_event(child);
        auto parent_event_info = subscribe_event(parent);

        auto window = zaf::Create<zaf::Window>();
        window->RootControl()->AddChild(parent);

        ASSERT_TRUE(child_event_info->is_raised);
        ASSERT_EQ(child_event_info->previous_window, nullptr);
        ASSERT_EQ(child_event_info->new_window, window);

        ASSERT_TRUE(parent_event_info->is_raised);
        ASSERT_EQ(parent_event_info->previous_window, nullptr);
        ASSERT_EQ(parent_event_info->new_window, window);
    }

    //Remove parent from a window, both parent and child should raise the event.
    {
        auto parent = zaf::Create<zaf::Control>();
        auto child = zaf::Create<zaf::Control>();
        parent->AddChild(child);

        auto window = zaf::Create<zaf::Window>();
        window->RootControl()->AddChild(parent);

        auto child_event_info = subscribe_event(child);
        auto parent_event_info = subscribe_event(parent);

        window->RootControl()->RemoveChild(parent);

        ASSERT_TRUE(child_event_info->is_raised);
        ASSERT_EQ(child_event_info->previous_window, window);
        ASSERT_EQ(child_event_info->new_window, nullptr);

        ASSERT_TRUE(parent_event_info->is_raised);
        ASSERT_EQ(parent_event_info->previous_window, window);
        ASSERT_EQ(parent_event_info->new_window, nullptr);
    }

    //Set parent to a different window, both parent and child should raise the event.
    {
        auto parent = zaf::Create<zaf::Control>();
        auto child = zaf::Create<zaf::Control>();
        parent->AddChild(child);

        auto window = zaf::Create<zaf::Window>();
        window->RootControl()->AddChild(parent);

        auto child_event_info = subscribe_event(child);
        auto parent_event_info = subscribe_event(parent);

        auto new_window = zaf::Create<zaf::Window>();
        new_window->RootControl()->AddChild(parent);

        ASSERT_TRUE(child_event_info->is_raised);
        ASSERT_EQ(child_event_info->previous_window, window);
        ASSERT_EQ(child_event_info->new_window, new_window);

        ASSERT_TRUE(parent_event_info->is_raised);
        ASSERT_EQ(parent_event_info->previous_window, window);
        ASSERT_EQ(parent_event_info->new_window, new_window);
    }

    //Set a control as root control of a window, the event should be raised.
    {
        auto window = zaf::Create<zaf::Window>();
        auto old_control_event_info = subscribe_event(window->RootControl());

        auto new_control = zaf::Create<zaf::Control>();
        auto new_control_event_info = subscribe_event(new_control);
        
        window->SetRootControl(new_control);

        ASSERT_TRUE(old_control_event_info->is_raised);
        ASSERT_EQ(old_control_event_info->previous_window, window);
        ASSERT_EQ(old_control_event_info->new_window, nullptr);

        ASSERT_TRUE(new_control_event_info->is_raised);
        ASSERT_EQ(new_control_event_info->previous_window, nullptr);
        ASSERT_EQ(new_control_event_info->new_window, window);
    }
}


TEST(ControlTest, RectChangedEvent) {

    struct EventInfo {
        bool rect_changed_raised{};
        zaf::Rect old_rect;
        bool position_changed_raised{};
        zaf::Point old_position;
        bool size_changed_raised{};
        zaf::Size old_size;
    };

    auto subscribe_event = [](const std::shared_ptr<zaf::Control>& control) {
    
        auto result = std::make_shared<EventInfo>();

        auto& subscriptions = zaf::Application::Instance().Subscriptions();
        subscriptions += control->RectChangedEvent().Subscribe(
            [result](const zaf::RectChangedInfo& event_info) {
        
            result->rect_changed_raised = true;
            result->old_rect = event_info.PreviousRect();
        });

        subscriptions += control->PositionChangedEvent().Subscribe(
            [result](const zaf::PositionChangedInfo& event_info) {
        
            result->position_changed_raised = true;
            result->old_position = event_info.PreviousPosition();
        });

        subscriptions += control->SizeChangedEvent().Subscribe(
            [result](const zaf::SizeChangedInfo& event_info) {

            result->size_changed_raised = true;
            result->old_size = event_info.PreviousSize();
        });

        return result;
    };

    auto control = zaf::Create<zaf::Control>();
    auto event_info = subscribe_event(control);

    control->SetRect(zaf::Rect{ 10, 10, 100, 100 });
    ASSERT_TRUE(event_info->rect_changed_raised);
    ASSERT_EQ(event_info->old_rect, zaf::Rect());
    ASSERT_TRUE(event_info->position_changed_raised);
    ASSERT_EQ(event_info->old_position, zaf::Point());
    ASSERT_TRUE(event_info->size_changed_raised);
    ASSERT_EQ(event_info->old_size, zaf::Size());

    *event_info = {};
    control->SetRect(zaf::Rect{ 10, 10, 100, 100 });
    ASSERT_FALSE(event_info->rect_changed_raised);
    ASSERT_FALSE(event_info->position_changed_raised);
    ASSERT_FALSE(event_info->size_changed_raised);

    *event_info = {};
    control->SetRect(zaf::Rect{ 20, 20, 200, 200 });
    ASSERT_TRUE(event_info->rect_changed_raised);
    ASSERT_EQ(event_info->old_rect, zaf::Rect(10, 10, 100, 100));
    ASSERT_TRUE(event_info->position_changed_raised);
    ASSERT_EQ(event_info->old_position, zaf::Point(10, 10));
    ASSERT_TRUE(event_info->size_changed_raised);
    ASSERT_EQ(event_info->old_size, zaf::Size(100, 100));

    *event_info = {};
    control->SetRect(zaf::Rect{ 30, 30, 200, 200 });
    ASSERT_TRUE(event_info->rect_changed_raised);
    ASSERT_EQ(event_info->old_rect, zaf::Rect(20, 20, 200, 200));
    ASSERT_TRUE(event_info->position_changed_raised);
    ASSERT_EQ(event_info->old_position, zaf::Point(20, 20));
    ASSERT_FALSE(event_info->size_changed_raised);

    *event_info = {};
    control->SetRect(zaf::Rect{ 30, 30, 300, 300 });
    ASSERT_TRUE(event_info->rect_changed_raised);
    ASSERT_EQ(event_info->old_rect, zaf::Rect(30, 30, 200, 200));
    ASSERT_FALSE(event_info->position_changed_raised);
    ASSERT_TRUE(event_info->size_changed_raised);
    ASSERT_EQ(event_info->old_size, zaf::Size(200, 200));
}


TEST(ControlTest, RemoveChildAtIndex) {

    auto parent = zaf::Create<zaf::Control>();
    ASSERT_THROW(parent->RemoveChildAtIndex(0), std::logic_error);

    for (int count = 0; count < 4; ++count) {
        auto child = zaf::Create<zaf::Control>();
        child->SetName(std::to_wstring(count));
        parent->AddChild(child);
    }

    parent->RemoveChildAtIndex(1);
    ASSERT_EQ(parent->ChildCount(), 3);
    ASSERT_EQ(parent->GetChildAtIndex(0)->Name(), L"0");
    ASSERT_EQ(parent->GetChildAtIndex(1)->Name(), L"2");
    ASSERT_EQ(parent->GetChildAtIndex(2)->Name(), L"3");

    parent->RemoveChildAtIndex(2);
    ASSERT_EQ(parent->ChildCount(), 2);
    ASSERT_EQ(parent->GetChildAtIndex(0)->Name(), L"0");
    ASSERT_EQ(parent->GetChildAtIndex(1)->Name(), L"2");

    parent->RemoveChildAtIndex(0);
    ASSERT_EQ(parent->ChildCount(), 1);
    ASSERT_EQ(parent->GetChildAtIndex(0)->Name(), L"2");

    parent->RemoveChildAtIndex(0);
    ASSERT_EQ(parent->ChildCount(), 0);
}


TEST(ControlTest, IsVisible) {

    auto control = zaf::Create<zaf::Control>();
    //Control is visible by default.
    ASSERT_TRUE(control->IsVisible());

    control->SetIsVisible(false);
    ASSERT_FALSE(control->IsVisible());

    //Changing visibility will raise IsVisibleChangedEvent.
    zaf::SubscriptionSet subs;
    bool is_event_raised{};
    subs += control->IsVisibleChangedEvent().Subscribe(
        [&](const zaf::IsVisibleChangedInfo& event_info) {

        is_event_raised = true;
        ASSERT_EQ(event_info.Source(), control);
    });

    control->SetIsVisible(true);
    ASSERT_TRUE(is_event_raised);

    //Setting the same value won't raise the event.
    is_event_raised = false;
    control->SetIsVisible(true);
    ASSERT_FALSE(is_event_raised);
}


TEST(ControlTest, IsVisibleInContext) {

    auto control = zaf::Create<zaf::Control>();
    //Control is visible if there is no parent.
    ASSERT_TRUE(control->IsVisibleInContext());

    control->SetIsVisible(false);
    ASSERT_FALSE(control->IsVisibleInContext());
    control->SetIsVisible(true);

    auto parent = zaf::Create<zaf::Control>();
    parent->AddChild(control);
    //Control is visible if the parent is not in window.
    ASSERT_TRUE(control->IsVisibleInContext());

    auto window = zaf::Create<zaf::Window>();
    //Root control of the window is visible.
    ASSERT_TRUE(window->RootControl()->IsVisibleInContext());

    window->RootControl()->AddChild(parent);
    //Control is visible after attaching to a window.
    ASSERT_TRUE(control->IsVisibleInContext());

    parent->SetIsVisible(false);
    //Control is invisible if parent is invisible.
    ASSERT_FALSE(control->IsVisibleInContext());
}


TEST(ControlTest, IsEnabled) {

    auto control = zaf::Create<zaf::Control>();
    //Control is enabled by default.
    ASSERT_TRUE(control->IsEnabled());

    //Set control to disabled.
    control->SetIsEnabled(false);
    ASSERT_FALSE(control->IsEnabled());

    //Changing enabled state will raise IsEnabledChangedEvent.
    zaf::SubscriptionSet subs;
    bool is_event_raised{};
    subs += control->IsEnabledChangedEvent().Subscribe(
        [&](const zaf::IsEnabledChangedInfo& event_info) {

        is_event_raised = true;
        ASSERT_EQ(event_info.Source(), control);
    });

    control->SetIsEnabled(true);
    ASSERT_TRUE(is_event_raised);

    //Setting the same value won't raise the event.
    is_event_raised = false;
    control->SetIsEnabled(true);
    ASSERT_FALSE(is_event_raised);
}


TEST(ControlTest, IsEnabledInContext) {

    auto control = zaf::Create<zaf::Control>();
    //Control is enabled if there is no parent.
    ASSERT_TRUE(control->IsEnabledInContext());

    //Control is disabled in context if it is disabled.
    control->SetIsEnabled(false);
    ASSERT_FALSE(control->IsEnabledInContext());
    control->SetIsEnabled(true);

    auto parent = zaf::Create<zaf::Control>();
    parent->AddChild(control);
    //Control is enabled if the parent is not in window.
    ASSERT_TRUE(control->IsEnabledInContext());

    auto window = zaf::Create<zaf::Window>();
    //Root control of the window is enabled.
    ASSERT_TRUE(window->RootControl()->IsEnabledInContext());

    window->RootControl()->AddChild(parent);
    //Control is enabled after attaching to a window.
    ASSERT_TRUE(control->IsEnabledInContext());

    parent->SetIsEnabled(false);
    //Control is disabled if parent is disabled.
    ASSERT_FALSE(control->IsEnabledInContext());
}


TEST(ControlTest, IsSelected) {

    auto control = zaf::Create<zaf::Control>();
    ASSERT_FALSE(control->IsSelected());

    control->SetIsSelected(true);
    ASSERT_TRUE(control->IsSelected());

    bool is_event_raised{};

    zaf::SubscriptionSet subs;
    subs += control->IsSelectedChangedEvent().Subscribe(
        [&](const zaf::IsSelectedChangedInfo& event_info) {
    
        is_event_raised = true;
        ASSERT_EQ(event_info.Source(), control);
    });

    control->SetIsSelected(false);
    ASSERT_TRUE(is_event_raised);

    is_event_raised = false;
    control->SetIsSelected(false);
    ASSERT_FALSE(is_event_raised);
}


TEST(ControlTest, IsSelectedInContext) {

    auto control = zaf::Create<zaf::Control>();
    ASSERT_FALSE(control->IsSelectedInContext());

    control->SetIsSelected(true);
    ASSERT_TRUE(control->IsSelectedInContext());
    control->SetIsSelected(false);

    auto parent = zaf::Create<zaf::Control>();
    parent->AddChild(control);
    ASSERT_FALSE(control->IsSelectedInContext());

    parent->SetIsSelected(true);
    ASSERT_TRUE(control->IsSelectedInContext());
}