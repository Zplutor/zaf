#include <gtest/gtest.h>
#include <format>
#include <zaf/base/as.h>
#include <zaf/control/control.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include "utility/test_window.h"

using namespace zaf;

TEST(ControlTest, IsFocused) {

    auto control = zaf::Create<zaf::Control>();

    //Control isn't focused by default.
    ASSERT_FALSE(control->IsFocused());

    //Can't set focus to a control which is not in a window.
    control->SetCanFocused(true);
    control->SetIsFocused(true);
    ASSERT_FALSE(control->IsFocused());

    auto window = zaf::Create<zaf::Window>();
    window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
    window->SetRect(zaf::Rect{ 0, 0, 100, 100 });

    window->RootControl()->AddChild(control);
    //Can't set focus to a control whose window handle is not created.
    control->SetIsFocused(true);
    ASSERT_FALSE(control->IsFocused());

    window->Show();

    control->SetIsFocused(true);
    ASSERT_TRUE(control->IsFocused());

    control->SetIsFocused(false);
    ASSERT_FALSE(control->IsFocused());

    window->Destroy();
}


TEST(ControlTest, IsInFocusedContext) {

    auto control = zaf::Create<zaf::Control>();
    control->SetCanFocused(true);
    ASSERT_FALSE(control->IsInFocusedContext());

    auto parent = zaf::Create<zaf::Control>();
    parent->SetCanFocused(true);
    parent->AddChild(control);
    ASSERT_FALSE(control->IsInFocusedContext());
    ASSERT_FALSE(parent->IsInFocusedContext());

    auto window = zaf::Create<zaf::Window>();
    window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
    window->SetRect(zaf::Rect{ 0, 0, 100, 100 });
    window->RootControl()->AddChild(parent);

    window->Show();

    control->SetIsFocused(true);
    ASSERT_TRUE(control->IsInFocusedContext());
    ASSERT_FALSE(parent->IsInFocusedContext());

    parent->SetIsFocused(true);
    ASSERT_TRUE(control->IsInFocusedContext());
    ASSERT_TRUE(parent->IsInFocusedContext());

    parent->SetIsFocused(false);
    ASSERT_FALSE(control->IsInFocusedContext());
    ASSERT_FALSE(parent->IsInFocusedContext());

    window->Destroy();
}


TEST(ControlTest, FocusEvents) {

    TestWithWindow([](Window& window) {

        std::vector<std::wstring> event_logs;

        auto focus_gained_handler = [&event_logs](const FocusGainedInfo& event_info) {
            event_logs.push_back(std::format(
                L"FocusGained {}",
                As<Control>(event_info.Source())->Name()));
        };

        auto focus_lost_handler = [&event_logs](const FocusLostInfo& event_info) {
            event_logs.push_back(std::format(
                L"FocusLost {}", 
                As<Control>(event_info.Source())->Name()));
        };

        auto focused_changed_handler = [&event_logs](const FocusedControlChangedInfo& event_info) {
            event_logs.push_back(std::format(L"FocusedControlChanged {}",
                event_info.PreviousFocusedControl() ? 
                event_info.PreviousFocusedControl()->Name() :
                L""));
        };

        SubscriptionSet subs;

        auto control1 = Create<Control>();
        control1->SetName(L"Control1");
        control1->SetCanFocused(true);
        subs += control1->FocusGainedEvent().Subscribe(focus_gained_handler);
        subs += control1->FocusLostEvent().Subscribe(focus_lost_handler);

        auto control2 = Create<Control>();
        control2->SetName(L"Control2");
        control2->SetCanFocused(true);
        subs += control2->FocusGainedEvent().Subscribe(focus_gained_handler);
        subs += control2->FocusLostEvent().Subscribe(focus_lost_handler);

        window.RootControl()->AddChildren({ control1, control2 });
        subs += window.FocusedControlChangedEvent().Subscribe(focused_changed_handler);

        //Set focus to Control1.
        event_logs.clear();
        control1->SetIsFocused(true);
        std::vector<std::wstring> expected_logs{
            L"FocusGained Control1",
            L"FocusedControlChanged "
        };
        ASSERT_EQ(event_logs, expected_logs);

        //Switch focus to Control2.
        event_logs.clear();
        control2->SetIsFocused(true);
        expected_logs = {
            L"FocusLost Control1",
            L"FocusGained Control2",
            L"FocusedControlChanged Control1"
        };
        ASSERT_EQ(event_logs, expected_logs);

        //Clear focus
        event_logs.clear();
        control2->SetIsFocused(false);
        expected_logs = {
            L"FocusLost Control2",
            L"FocusedControlChanged Control2"
        };
        ASSERT_EQ(event_logs, expected_logs);
    });
}


/*
Special test case: set focus back to the original control on its focus lost event.
*/
TEST(ControlTest, SetBackFocusOnFocusLost) {

    TestWithWindow([](Window& window) {
    
        auto control1 = Create<Control>();
        control1->SetCanFocused(true);

        auto control2 = Create<Control>();
        control2->SetCanFocused(true);
        
        window.RootControl()->AddChildren({ control1, control2 });
        control1->SetIsFocused(true);
        ASSERT_TRUE(control1->IsFocused());
        ASSERT_EQ(window.FocusedControl(), control1);

        auto sub = control1->FocusLostEvent().Subscribe(
            [control1](const FocusLostInfo& event_info) {

            //Set focus back to the control.
            control1->SetIsFocused(true);
        });

        control2->SetIsFocused(true);

        ASSERT_TRUE(control1->IsFocused());
        ASSERT_FALSE(control2->IsFocused());
        ASSERT_EQ(window.FocusedControl(), control1);
    });
}