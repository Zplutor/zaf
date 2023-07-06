#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/control/label.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/control/linear_box.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

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


TEST(ControlTest, AutoSizeOnSetRect) {

    auto label = zaf::Create<zaf::Label>();
    label->SetText(L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    label->SetWordWrapping(zaf::WordWrapping::Character);
    label->SetSize(zaf::Size{ 300, 1000 });
    label->SetAutoHeight(true);
    ASSERT_EQ(label->Width(), 300);

    auto preferred_size = label->CalculatePreferredSize(zaf::Size{ 30, 1000 });
    label->SetWidth(30);
    ASSERT_EQ(label->Width(), 30);
    ASSERT_EQ(label->Height(), preferred_size.height);
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


TEST(ControlTest, WindowChangedEvent) {

    //Add/Remove a child to parent which is not in a window, should not raise the event.
    {
        zaf::SubscriptionSet subscriptions;

        auto parent = zaf::Create<zaf::Control>();
        auto child = zaf::Create<zaf::Control>();

        subscriptions += child->WindowChangedEvent().Subscribe(
            [](const zaf::WindowChangedInfo& event_info) {
            GTEST_FAIL();
        });
        parent->AddChild(child);

        //Remove a child from parent which is not in a window, should not raise the event.
        parent->RemoveChild(child);
    }

    //Add parent to a window, both parent and child should raise the event.
    {
        zaf::SubscriptionSet subscriptions;

        auto parent = zaf::Create<zaf::Control>();
        auto child = zaf::Create<zaf::Control>();
        parent->AddChild(child);

        std::optional<std::shared_ptr<zaf::Window>> child_previous_window;
        subscriptions += child->WindowChangedEvent().Subscribe(
            [&child_previous_window](const zaf::WindowChangedInfo& event_info) {
            child_previous_window = event_info.PreviousWindow();
        });

        std::optional<std::shared_ptr<zaf::Window>> parent_previous_window;
        subscriptions += parent->WindowChangedEvent().Subscribe(
            [&parent_previous_window](const zaf::WindowChangedInfo& event_info) {
            parent_previous_window = event_info.PreviousWindow();
        });

        auto window = zaf::Create<zaf::Window>();
        window->RootControl()->AddChild(parent);

        ASSERT_TRUE(child_previous_window.has_value());
        ASSERT_EQ(*child_previous_window, nullptr);
        ASSERT_TRUE(parent_previous_window.has_value());
        ASSERT_EQ(*parent_previous_window, nullptr);
    }

    //Remove parent from a window, both parent and child should raise the event.
    {
        zaf::SubscriptionSet subscriptions;

        auto parent = zaf::Create<zaf::Control>();
        auto child = zaf::Create<zaf::Control>();
        parent->AddChild(child);

        auto window = zaf::Create<zaf::Window>();
        window->RootControl()->AddChild(parent);

        std::optional<std::shared_ptr<zaf::Window>> child_previous_window;
        subscriptions += child->WindowChangedEvent().Subscribe(
            [&child_previous_window](const zaf::WindowChangedInfo& event_info) {
            child_previous_window = event_info.PreviousWindow();
        });

        std::optional<std::shared_ptr<zaf::Window>> parent_previous_window;
        subscriptions += parent->WindowChangedEvent().Subscribe(
            [&parent_previous_window](const zaf::WindowChangedInfo& event_info) {
            parent_previous_window = event_info.PreviousWindow();
        });

        window->RootControl()->RemoveChild(parent);

        ASSERT_TRUE(child_previous_window.has_value());
        ASSERT_EQ(*child_previous_window, window);
        ASSERT_TRUE(parent_previous_window.has_value());
        ASSERT_EQ(*parent_previous_window, window);
    }

    //Set parent to a different window, both parent and child should raise the event.
    {
        zaf::SubscriptionSet subscriptions;

        auto parent = zaf::Create<zaf::Control>();
        auto child = zaf::Create<zaf::Control>();
        parent->AddChild(child);

        auto window = zaf::Create<zaf::Window>();
        window->RootControl()->AddChild(parent);

        std::optional<std::shared_ptr<zaf::Window>> child_previous_window;
        subscriptions += child->WindowChangedEvent().Subscribe(
            [&child_previous_window](const zaf::WindowChangedInfo& event_info) {
            child_previous_window = event_info.PreviousWindow();
        });

        std::optional<std::shared_ptr<zaf::Window>> parent_previous_window;
        subscriptions += parent->WindowChangedEvent().Subscribe(
            [&parent_previous_window](const zaf::WindowChangedInfo& event_info) {
            parent_previous_window = event_info.PreviousWindow();
        });

        auto new_window = zaf::Create<zaf::Window>();
        new_window->RootControl()->AddChild(parent);

        ASSERT_TRUE(child_previous_window.has_value());
        ASSERT_EQ(*child_previous_window, window);
        ASSERT_TRUE(parent_previous_window.has_value());
        ASSERT_EQ(*parent_previous_window, window);
    }
}