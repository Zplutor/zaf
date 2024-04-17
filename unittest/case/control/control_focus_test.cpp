#include <gtest/gtest.h>
#include <format>
#include <zaf/base/as.h>
#include <zaf/control/control.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include "utility/test_window.h"

using namespace zaf;

namespace {

class ControlFocusTest : public testing::Test, SubscriptionHost {
public:
    static void SetUpTestCase() {
        test_window_ = zaf::Create<zaf::Window>();
        test_window_->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
        test_window_->SetRect(zaf::Rect{ 0, 0, 100, 100 });
        test_window_->RootControl()->SetName(L"Root");
        test_window_->Show();
    }

    static void TearDownTestCase() {
        test_window_->Destroy();
        test_window_.reset();
    }

protected:
    void SetUp() override {

        InitializeControls();
        InitializeSubscriptions();

        ::SetFocus(test_window_->Handle());
    }

    void TearDown() override {

        test_window_->RootControl()->RemoveAllChildren();
    }

    bool CheckEventLogs(const std::vector<std::wstring>& logs) {
        bool result = event_logs_ == logs;
        event_logs_.clear();
        return result;
    }

    static const std::shared_ptr<Window>& TestWindow() {
        return test_window_;
    }

    const std::shared_ptr<Control>& Control1() {
        return control1_;
    }

    const std::shared_ptr<Control>& Control2() {
        return control2_;
    }

private:
    void InitializeControls() {

        control1_ = Create<Control>();
        control1_->SetName(L"Control1");
        control1_->SetCanFocused(true);

        control2_ = Create<Control>();
        control2_->SetName(L"Control2");
        control2_->SetCanFocused(true);

        test_window_->RootControl()->AddChildren({ control1_, control2_ });
    }

    void InitializeSubscriptions() {

        auto pre_focus_gained_handler = [this](const PreFocusGainedInfo& event_info) {
            event_logs_.push_back(std::format(
                L"PreFocusGained {} {}",
                As<Control>(event_info.Source())->Name(),
                As<Control>(event_info.Sender())->Name()));
        };

        auto focus_gained_handler = [this](const FocusGainedInfo& event_info) {
            event_logs_.push_back(std::format(
                L"FocusGained {} {}",
                As<Control>(event_info.Source())->Name(),
                As<Control>(event_info.Sender())->Name()));
        };

        auto pre_focus_lost_handler = [this](const PreFocusLostInfo& event_info) {
            event_logs_.push_back(std::format(
                L"PreFocusLost {} {}",
                As<Control>(event_info.Source())->Name(),
                As<Control>(event_info.Sender())->Name()));
        };

        auto focus_lost_handler = [this](const FocusLostInfo& event_info) {
            event_logs_.push_back(std::format(
                L"FocusLost {} {}",
                As<Control>(event_info.Source())->Name(),
                As<Control>(event_info.Sender())->Name()));
        };

        for (const auto& each_control : { control1_, control2_, test_window_->RootControl() }) {

            Subscriptions() += each_control->PreFocusGainedEvent().Subscribe(
                pre_focus_gained_handler);
            Subscriptions() += each_control->FocusGainedEvent().Subscribe(focus_gained_handler);
            Subscriptions() += each_control->PreFocusLostEvent().Subscribe(pre_focus_lost_handler);
            Subscriptions() += each_control->FocusLostEvent().Subscribe(focus_lost_handler);
        }

        Subscriptions() += test_window_->FocusedControlChangedEvent().Subscribe(
            [this](const FocusedControlChangedInfo& event_info) {
                event_logs_.push_back(std::format(L"FocusedControlChanged {}",
                event_info.PreviousFocusedControl() ?
                event_info.PreviousFocusedControl()->Name() :
                L""));
            });
    }

private:
    static std::shared_ptr<Window> test_window_;

    std::shared_ptr<Control> control1_;
    std::shared_ptr<Control> control2_;
    std::vector<std::wstring> event_logs_;
};

std::shared_ptr<Window> ControlFocusTest::test_window_;

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


TEST_F(ControlFocusTest, CheckFocusInEvents) {

    Control1()->SetIsFocused(true);

    SubscriptionSet subs;
    subs += Control1()->PreFocusLostEvent().Subscribe([this](const PreFocusLostInfo& event_info) {

        ASSERT_FALSE(As<Control>(event_info.Source())->IsFocused());

        ASSERT_EQ(event_info.GainedFocusControl(), Control2());
        ASSERT_EQ(TestWindow()->FocusedControl(), Control2());
        ASSERT_TRUE(Control2()->IsFocused());
    });

    subs += Control1()->FocusLostEvent().Subscribe([this](const FocusLostInfo& event_info) {

        ASSERT_FALSE(As<Control>(event_info.Source())->IsFocused());

        ASSERT_EQ(event_info.GainedFocusControl(), Control2());
        ASSERT_EQ(TestWindow()->FocusedControl(), Control2());
        ASSERT_TRUE(Control2()->IsFocused());
    });

    subs += Control2()->PreFocusGainedEvent().Subscribe(
        [this](const PreFocusGainedInfo& event_info) {

        ASSERT_TRUE(As<Control>(event_info.Source())->IsFocused());

        ASSERT_EQ(event_info.LostFocusControl(), Control1());
        ASSERT_EQ(TestWindow()->FocusedControl(), Control2());
        ASSERT_FALSE(Control1()->IsFocused());
    });

    subs += Control2()->FocusGainedEvent().Subscribe([this](const FocusGainedInfo& event_info) {
    
        ASSERT_TRUE(As<Control>(event_info.Source())->IsFocused());

        ASSERT_EQ(event_info.LostFocusControl(), Control1());
        ASSERT_EQ(TestWindow()->FocusedControl(), Control2());
        ASSERT_FALSE(Control1()->IsFocused());
    });

    Control2()->SetIsFocused(true);
}


TEST_F(ControlFocusTest, FocusEvents_NoReentrant) {

    //Set focus to Control1.
    Control1()->SetIsFocused(true);
    ASSERT_TRUE(CheckEventLogs({
        L"PreFocusGained Control1 Root",
        L"PreFocusGained Control1 Control1",
        L"FocusGained Control1 Control1",
        L"FocusGained Control1 Root",
        L"FocusedControlChanged ",
    }));

    //Switch focus to Control2.
    Control2()->SetIsFocused(true);
    ASSERT_TRUE(CheckEventLogs({
        L"PreFocusLost Control1 Root",
        L"PreFocusLost Control1 Control1",
        L"FocusLost Control1 Control1",
        L"FocusLost Control1 Root",
        L"PreFocusGained Control2 Root",
        L"PreFocusGained Control2 Control2",
        L"FocusGained Control2 Control2",
        L"FocusGained Control2 Root",
        L"FocusedControlChanged Control1"
    }));

    //Clear focus
    Control2()->SetIsFocused(false);
    ASSERT_TRUE(CheckEventLogs({
        L"PreFocusLost Control2 Root",
        L"PreFocusLost Control2 Control2",
        L"FocusLost Control2 Control2",
        L"FocusLost Control2 Root",
        L"FocusedControlChanged Control2"
    }));
}


/*
Cancel the focus on its focus gained event.
*/
TEST_F(ControlFocusTest, CancelFocusOnFocusGained) {

    auto sub = Control1()->FocusGainedEvent().Subscribe([this](const FocusGainedInfo& event_info) {
        Control1()->SetIsFocused(false);
    });

    Control1()->SetIsFocused(true);

    ASSERT_FALSE(Control1()->IsFocused());
    ASSERT_EQ(TestWindow()->FocusedControl(), nullptr);

    ASSERT_TRUE(CheckEventLogs({
        L"PreFocusGained Control1 Root",
        L"PreFocusGained Control1 Control1",
        L"FocusGained Control1 Control1",
        L"PreFocusLost Control1 Root",
        L"PreFocusLost Control1 Control1",
        L"FocusLost Control1 Control1",
        L"FocusLost Control1 Root",
    }));
}


/*
Set focus back to the original control on its focus lost event.
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
        L"PreFocusGained Control1 Root",
        L"PreFocusGained Control1 Control1",
        L"FocusGained Control1 Control1",
        L"FocusGained Control1 Root",
        L"FocusedControlChanged ",
        L"PreFocusLost Control1 Root",
        L"PreFocusLost Control1 Control1",
        L"FocusLost Control1 Control1",
        L"PreFocusGained Control1 Root",
        L"PreFocusGained Control1 Control1",
        L"FocusGained Control1 Control1",
        L"FocusGained Control1 Root",
    }));
}