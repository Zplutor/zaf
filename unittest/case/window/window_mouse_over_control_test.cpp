#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>
#include <zaf/graphic/dpi.h>
#include <zaf/window/window.h>

namespace {

class WindowMouseOverControlTest : public testing::Test {
public:
    static void SetUpTestCase() {
    
        s_window = zaf::Create<zaf::Window>();
        s_window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
        s_window->SetPosition({});
        s_window->SetContentSize(zaf::Size{ 200, 200 });
        s_window->Show();

        s_window->RootControl()->SetLayouter(zaf::Create<zaf::VerticalLayouter>());
    }

    static void TearDownTestCase() {

        s_window->Destroy();
    }

protected:
    static const std::shared_ptr<zaf::Window>& TestWindow() {
        return s_window;
    }

protected:
    void SetUp() override {

        test_control_ = zaf::Create<zaf::Control>();
        test_control_->SetRect(zaf::Rect{ 0, 0, 100, 100 });

        parent_control_ = zaf::Create<zaf::Control>();
        parent_control_->AddChild(test_control_);

        s_window->RootControl()->AddChild(parent_control_);
    }

    void TearDown() override {

        test_control_ = nullptr;
        parent_control_ = nullptr;
        s_window->RootControl()->RemoveAllChildren();
    }

    const std::shared_ptr<zaf::Control>& ParentControl() const {
        return parent_control_;
    }

    const std::shared_ptr<zaf::Control>& TestControl() const {
        return test_control_;
    }

    void SendMessageToSetTestControlAsMouseOverControl() {
        SendMouseMoveMessage(50, 50);
    }

    void SendMouseMoveMessage(float x, float y) {
        int x_in_pixels = static_cast<int>(zaf::FromDIPs(x, s_window->GetDPI()));
        int y_in_pixels = static_cast<int>(zaf::FromDIPs(y, s_window->GetDPI()));
        SendMessage(s_window->Handle(), WM_MOUSEMOVE, 0, MAKELPARAM(x_in_pixels, y_in_pixels));
    }

private:
    static std::shared_ptr<zaf::Window> s_window;

private:
    std::shared_ptr<zaf::Control> parent_control_;
    std::shared_ptr<zaf::Control> test_control_;
};


std::shared_ptr<zaf::Window> WindowMouseOverControlTest::s_window;

}


TEST_F(WindowMouseOverControlTest, SetMouseOverControl) {

    //There is no mouse over control at first.
    ASSERT_EQ(TestWindow()->MouseOverControl(), nullptr);

    //Send a mouse move message to set mouse over control.
    SendMessageToSetTestControlAsMouseOverControl();
    ASSERT_EQ(TestWindow()->MouseOverControl(), TestControl());
}


TEST_F(WindowMouseOverControlTest, NotInPosition) {

    SendMouseMoveMessage(150, 150);
    ASSERT_EQ(TestWindow()->MouseOverControl(), ParentControl());
}


TEST_F(WindowMouseOverControlTest, DisabledControl) {

    TestControl()->SetIsEnabled(false);

    SendMessageToSetTestControlAsMouseOverControl();
    ASSERT_EQ(TestWindow()->MouseOverControl(), ParentControl());
}


TEST_F(WindowMouseOverControlTest, InvisibleControl) {

    TestControl()->SetIsVisible(false);

    SendMessageToSetTestControlAsMouseOverControl();
    ASSERT_EQ(TestWindow()->MouseOverControl(), ParentControl());
}


TEST_F(WindowMouseOverControlTest, DisableMouseOverControl) {

    SendMessageToSetTestControlAsMouseOverControl();

    TestControl()->SetIsEnabled(false);
    ASSERT_EQ(TestWindow()->MouseOverControl(), nullptr);
}


TEST_F(WindowMouseOverControlTest, DisableParentOfMouseOverControl) {

    SendMessageToSetTestControlAsMouseOverControl();

    ParentControl()->SetIsEnabled(false);
    ASSERT_EQ(TestWindow()->MouseOverControl(), nullptr);
}


TEST_F(WindowMouseOverControlTest, HideMouseOverControl) {

    SendMessageToSetTestControlAsMouseOverControl();

    TestControl()->SetIsVisible(false);
    ASSERT_EQ(TestWindow()->MouseOverControl(), nullptr);
}


TEST_F(WindowMouseOverControlTest, HideParentOfMouseOverControl) {

    SendMessageToSetTestControlAsMouseOverControl();

    ParentControl()->SetIsVisible(false);
    ASSERT_EQ(TestWindow()->MouseOverControl(), nullptr);
}


TEST_F(WindowMouseOverControlTest, HandleMouseMoveMessage) {

    //Mouse over control wouldn't change if pre mouse move event is handled.
    zaf::rx::SubscriptionBag subs;
    subs += ParentControl()->PreMouseMoveEvent().Subscribe(
        [](const zaf::PreMouseMoveInfo& event_info) {
    
        event_info.MarkAsHandled();
    });

    SendMessageToSetTestControlAsMouseOverControl();
    ASSERT_EQ(TestWindow()->MouseOverControl(), nullptr);
}