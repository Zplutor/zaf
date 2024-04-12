#include <gtest/gtest.h>
#include <format>
#include <zaf/base/as.h>
#include <zaf/control/control.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include "utility/test_window.h"

using namespace zaf;

namespace {

class ControlFocusTest : public testing::Test {
public:
    static void SetUpTestCase() {

        auto focus_gained_handler = [](const FocusGainedInfo& event_info) {
            event_logs_.push_back(std::format(
                L"FocusGained {}",
                As<Control>(event_info.Source())->Name()));
        };

        auto focus_lost_handler = [](const FocusLostInfo& event_info) {
            event_logs_.push_back(std::format(
                L"FocusLost {}",
                As<Control>(event_info.Source())->Name()));
        };

        auto focused_changed_handler = [](const FocusedControlChangedInfo& event_info) {
            event_logs_.push_back(std::format(L"FocusedControlChanged {}",
                event_info.PreviousFocusedControl() ?
                event_info.PreviousFocusedControl()->Name() :
                L""));
        };

        test_window_ = zaf::Create<zaf::Window>();
        test_window_->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
        test_window_->SetRect(zaf::Rect{ 0, 0, 100, 100 });

        control1_ = Create<Control>();
        control1_->SetName(L"Control1");
        control1_->SetCanFocused(true);
        test_window_->Subscriptions() += 
            control1_->FocusGainedEvent().Subscribe(focus_gained_handler);
        test_window_->Subscriptions() += 
            control1_->FocusLostEvent().Subscribe(focus_lost_handler);

        control2_ = Create<Control>();
        control2_->SetName(L"Control2");
        control2_->SetCanFocused(true);
        test_window_->Subscriptions() += 
            control2_->FocusGainedEvent().Subscribe(focus_gained_handler);
        test_window_->Subscriptions() += 
            control2_->FocusLostEvent().Subscribe(focus_lost_handler);

        test_window_->RootControl()->AddChildren({ control1_, control2_ });
        test_window_->Subscriptions() += 
            test_window_->FocusedControlChangedEvent().Subscribe(focused_changed_handler);

        test_window_->Show();
    }

    static void TearDownTestCase() {

        test_window_->Destroy();
        test_window_.reset();

        control1_.reset();
        control2_.reset();
    }

protected:
    void SetUp() override {

        event_logs_.clear();
        control1_->SetIsFocused(false);
        control2_->SetIsFocused(false);

        ::SetFocus(test_window_->Handle());
    }

    static bool CheckEventLogs(const std::vector<std::wstring>& logs) {
        //It's wired that VS doesn't display the value of static variables, so we use a local 
        //reference to make it visible.
        const auto& actual_logs = event_logs_;
        bool result = actual_logs == logs;
        event_logs_.clear();
        return result;
    }

    static const std::shared_ptr<Window>& TestWindow() {
        return test_window_;
    }

    static const std::shared_ptr<Control>& Control1() {
        return control1_;
    }

    static const std::shared_ptr<Control>& Control2() {
        return control2_;
    }

private:
    static std::shared_ptr<Window> test_window_;
    static std::shared_ptr<Control> control1_;
    static std::shared_ptr<Control> control2_;
    static std::vector<std::wstring> event_logs_;
};

std::shared_ptr<Window> ControlFocusTest::test_window_;
std::shared_ptr<Control> ControlFocusTest::control1_;
std::shared_ptr<Control> ControlFocusTest::control2_;
std::vector<std::wstring> ControlFocusTest::event_logs_;

}

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


TEST_F(ControlFocusTest, FocusEvents_NoReentrant) {

    //Set focus to Control1.
    Control1()->SetIsFocused(true);
    ASSERT_TRUE(CheckEventLogs({
        L"FocusGained Control1",
        L"FocusedControlChanged ",
    }));

    //Switch focus to Control2.
    Control2()->SetIsFocused(true);
    ASSERT_TRUE(CheckEventLogs({
        L"FocusLost Control1",
        L"FocusGained Control2",
        L"FocusedControlChanged Control1"
    }));

    //Clear focus
    Control2()->SetIsFocused(false);
    ASSERT_TRUE(CheckEventLogs({
        L"FocusLost Control2",
        L"FocusedControlChanged Control2"
    }));
}


/*
Special test case: cancel the focus on its focus gained event.
*/
TEST_F(ControlFocusTest, CancelFocusOnFocusGained) {

    auto sub = Control1()->FocusGainedEvent().Subscribe([this](const FocusGainedInfo& event_info) {
        Control1()->SetIsFocused(false);
    });

    Control1()->SetIsFocused(true);

    ASSERT_FALSE(Control1()->IsFocused());
    ASSERT_EQ(TestWindow()->FocusedControl(), nullptr);

    ASSERT_TRUE(CheckEventLogs({
        L"FocusGained Control1",
        L"FocusedControlChanged ",
        L"FocusLost Control1",
        L"FocusedControlChanged Control1",
    }));
}


/*
Special test case: set focus back to the original control on its focus lost event.
*/
TEST_F(ControlFocusTest, SetBackFocusOnFocusLost) {

    Control1()->SetIsFocused(true);
    ASSERT_TRUE(Control1()->IsFocused());
    ASSERT_EQ(TestWindow()->FocusedControl(), Control1());

    auto sub = Control1()->FocusLostEvent().Subscribe([this](const FocusLostInfo& event_info) {
        //Set focus back to the control.
        Control1()->SetIsFocused(true);
    });

    Control2()->SetIsFocused(true);

    ASSERT_TRUE(Control1()->IsFocused());
    ASSERT_FALSE(Control2()->IsFocused());
    ASSERT_EQ(TestWindow()->FocusedControl(), Control1());

    ASSERT_TRUE(CheckEventLogs({
        L"FocusGained Control1",
        L"FocusedControlChanged ",
        L"FocusLost Control1",
        L"FocusGained Control2",
        L"FocusedControlChanged Control1",
        L"FocusLost Control2",
        L"FocusGained Control1",
        L"FocusedControlChanged Control2",
    }));
}