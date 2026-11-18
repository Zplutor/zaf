#include <gtest/gtest.h>
#include <zaf/application.h>
#include <zaf/base/as.h>
#include <zaf/control/control.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

/*
While removing a child, it needs to raise the parent changed event, which requires a shared pointer
of the parent. RemoveChild() may mistakenly call shared_from_this() on the parent to get the shared
pointer, but if the parent is being destructed, it will cause a crash. This test ensures that 
removing a child in the destructor does not cause a crash.
*/
TEST(ControlTest, RemoveChildInDestructor) {

    class TestControl : public zaf::Control {
    public:
        ~TestControl() {
            RemoveChild(child_);
        }

    protected:
        void Initialize() override {
            __super::Initialize();
            child_ = zaf::Create<zaf::Control>();
            AddChild(child_);
        }

    private:
        std::shared_ptr<zaf::Control> child_;
    };

    auto test_control = zaf::Create<TestControl>();
    test_control.reset();
}


TEST(ControlTest, WindowChangedEvent) {

    struct EventInfo {
        bool is_raised{};
        std::shared_ptr<zaf::Window> previous_window;
        std::shared_ptr<zaf::Window> new_window;
    };

    auto subscribe_event = [](const std::shared_ptr<zaf::Control>& control) {

        auto result = std::make_shared<EventInfo>();
        zaf::Application::Instance().Disposables() += control->WindowChangedEvent().Subscribe(
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


TEST(ControlTest, RemoveChildAtIndex) {

    auto parent = zaf::Create<zaf::Control>();
    ASSERT_NO_THROW(parent->RemoveChildAtIndex(0));

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
