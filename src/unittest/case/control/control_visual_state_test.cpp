#include <gtest/gtest.h>
#include <zaf/base/as.h>
#include <zaf/control/label.h>
#include <zaf/creation.h>
#include "utility/test_window.h"

using namespace zaf;

TEST(ControlTest, UpdateVisualStateCase) {

    TestWithWindow([](Window& window) {
    
        auto control = Create<Control>();
        control->SetSize(Size{ 100, 100 });

        int update_count{};
        auto sub = control->StyleUpdateEvent().Subscribe(
            [&update_count](const StyleUpdateInfo& event_info) {
            ++update_count;
        });

        //Changing parent will raise visual state update event.
        window.RootControl()->AddChild(control);
        UpdateWindow(window.Handle());
        ASSERT_EQ(update_count, 1);

        //Changing IsEnabled will raise visual state update event.
        update_count = 0;
        control->SetIsEnabled(false);
        control->NeedRepaint();
        UpdateWindow(window.Handle());
        ASSERT_EQ(update_count, 1);

        //Changing IsFocused will raise visual state update event.
        control->SetIsEnabled(true);
        control->SetCanFocused(true);
        UpdateWindow(window.Handle());
        update_count = 0;
        control->SetIsFocused(true);
        control->NeedRepaint();
        UpdateWindow(window.Handle());
        ASSERT_EQ(update_count, 1);

        //Changing IsVisible to true will raise visual state update event.
        control->SetIsVisible(false);
        UpdateWindow(window.Handle());
        update_count = 0;
        control->SetIsVisible(true);
        control->NeedRepaint();
        UpdateWindow(window.Handle());
        ASSERT_EQ(update_count, 1);

        //Changing IsSelected will raise visual state update event.
        update_count = 0;
        control->SetIsSelected(true);
        control->NeedRepaint();
        UpdateWindow(window.Handle());
        ASSERT_EQ(update_count, 1);

        //Changing mouse over will raise visual state update event.
        update_count = 0;
        window.Messager().Send(WM_MOUSEMOVE, 0, 0);
        control->NeedRepaint();
        UpdateWindow(window.Handle());
        ASSERT_EQ(update_count, 1);
    });
}


TEST(ControlTest, NoPaintWhenUpdatingVisualState) {

    TestWithWindow([](Window& window) {
    
        auto label = Create<Label>();
        label->SetSize(Size{ 100, 30 });
        label->SetIsEnabled(true);
        label->SetTextColor(Color::White());

        auto label_sub = label->StyleUpdateEvent().Subscribe(
            [](const StyleUpdateInfo& event_info) {

            auto label = As<Label>(event_info.Source());
            //This will make a NeedRepaint call.
            label->SetTextColor(Color::Black());
        });

        window.RootControl()->AddChild(label);

        int paint_count{};
        auto window_sub = window.MessageReceivedEvent().Subscribe(
            [&paint_count](const MessageReceivedInfo& event_info) {
            if (event_info.Message().ID() == WM_PAINT) {
                paint_count++;
            }
        });

        //This will update visual state.
        label->SetIsEnabled(false);

        //Trigger the WM_PAINT mandatorily.
        UpdateWindow(window.Handle());

        ASSERT_EQ(paint_count, 1);

        BOOL has_update_rect = GetUpdateRect(window.Handle(), nullptr, FALSE);
        ASSERT_FALSE(has_update_rect);
    });
}