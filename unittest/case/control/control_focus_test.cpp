#include <gtest/gtest.h>
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