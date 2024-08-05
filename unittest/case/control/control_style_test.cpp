#include <gtest/gtest.h>
#include <zaf/base/as.h>
#include <zaf/control/label.h>
#include <zaf/control/linear_box.h>
#include <zaf/creation.h>
#include "utility/test_window.h"

using namespace zaf;

namespace {

class ControlStyleTest : public testing::Test, public SubscriptionHost {
public:
    static void SetUpTestCase() {
        test_window_ = zaf::Create<zaf::Window>();
        test_window_->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
        test_window_->SetRect(zaf::Rect{ 0, 0, 100, 100 });
        test_window_->Show();
    }

    static void TearDownTestCase() {
        test_window_->Destroy();
        test_window_.reset();
    }

protected:
    void SetUp() override {

        leaf1_ = Create<Control>();
        leaf1_->SetRect(Rect{ 0, 0, 100, 100 });

        leaf2_ = Create<Control>();
        leaf2_->SetRect(Rect{ 1000, 0, 100, 100 });

        stem_ = Create<Control>();
        stem_->AddChildren({ leaf1_, leaf2_ });
        stem_->SetRect(Rect{ 0, 0, 100, 100 });

        test_window_->RootControl()->AddChild(stem_);

        //Consume any repaint requirement.
        RepaintWindow();

        Subscriptions() += leaf1_->StyleUpdateEvent().Subscribe(std::bind([this]() {
            leaf1_update_count_++;
        }));

        Subscriptions() += leaf2_->StyleUpdateEvent().Subscribe(std::bind([this]() {
            leaf2_update_count_++;
        }));

        Subscriptions() += stem_->StyleUpdateEvent().Subscribe(std::bind([this]() {
            stem_update_count_++;
        }));

        Subscriptions() += test_window_->RootControl()->StyleUpdateEvent().Subscribe(
            std::bind([this]() {
                root_update_count_++;
            }));
    }

    void TearDown() override {
        test_window_->RootControl()->RemoveAllChildren();
    }

    void RepaintWindow() {
        UpdateWindow(test_window_->Handle());
    }

    const std::shared_ptr<Control>& Stem() const {
        return stem_;
    }

    int RootUpdateCount() const {
        return root_update_count_;
    }

    int StemUpdateCount() const {
        return stem_update_count_;
    }

    int Leaf1UpdateCount() const {
        return leaf1_update_count_;
    }

    int Leaf2UpdateCount() const {
        return leaf2_update_count_;
    }

private:
    static std::shared_ptr<Window> test_window_;

    int root_update_count_{};

    std::shared_ptr<Control> stem_;
    int stem_update_count_{};

    //leaf1 is within the visible area.
    std::shared_ptr<Control> leaf1_;
    int leaf1_update_count_{};

    //leaf2 is outside the visible area.
    std::shared_ptr<Control> leaf2_;
    int leaf2_update_count_{};
};

std::shared_ptr<Window> ControlStyleTest::test_window_;

}


TEST_F(ControlStyleTest, UpdateOnParentChange) {

    auto new_control = Create<Control>();
    new_control->SetRect(Rect{ 0, 0, 50, 50 });

    int new_control_update_count{};
    Subscriptions() += new_control->StyleUpdateEvent().Subscribe(std::bind([&]() {
        ++new_control_update_count;
    }));

    Stem()->AddChild(new_control);
    RepaintWindow();

    ASSERT_EQ(new_control_update_count, 1);
    ASSERT_EQ(RootUpdateCount(), 0);
    ASSERT_EQ(StemUpdateCount(), 0);
    ASSERT_EQ(Leaf1UpdateCount(), 0);
    ASSERT_EQ(Leaf2UpdateCount(), 0);
}


TEST_F(ControlStyleTest, UpdateOnIsEnabledChange) {

    Stem()->SetIsEnabled(false);
    RepaintWindow();

    ASSERT_EQ(RootUpdateCount(), 0);
    ASSERT_EQ(StemUpdateCount(), 1);
    ASSERT_EQ(Leaf1UpdateCount(), 1);
    ASSERT_EQ(Leaf2UpdateCount(), 1);
}


TEST(ControlTest, UpdateStyleOnParentChange) {

    TestWithWindow([](Window& window) {
    
        auto control = Create<Control>();
        control->SetSize(Size{ 100, 100 });

        int update_count{};
        auto sub = control->StyleUpdateEvent().Subscribe(
            [&update_count](const StyleUpdateInfo& event_info) {
            ++update_count;
        });

        //Changing parent will update style.
        window.RootControl()->AddChild(control);
        UpdateWindow(window.Handle());
        ASSERT_EQ(update_count, 1);

        //Changing IsEnabled will update style.
        update_count = 0;
        control->SetIsEnabled(false);
        control->NeedRepaint();
        UpdateWindow(window.Handle());
        ASSERT_EQ(update_count, 1);

        //Changing IsFocused will update style.
        control->SetIsEnabled(true);
        control->SetCanFocused(true);
        UpdateWindow(window.Handle());
        update_count = 0;
        control->SetIsFocused(true);
        control->NeedRepaint();
        UpdateWindow(window.Handle());
        ASSERT_EQ(update_count, 1);

        //Changing IsVisible to true will update style.
        control->SetIsVisible(false);
        UpdateWindow(window.Handle());
        update_count = 0;
        control->SetIsVisible(true);
        control->NeedRepaint();
        UpdateWindow(window.Handle());
        ASSERT_EQ(update_count, 1);

        //Changing IsSelected will update style.
        update_count = 0;
        control->SetIsSelected(true);
        control->NeedRepaint();
        UpdateWindow(window.Handle());
        ASSERT_EQ(update_count, 1);

        //Changing mouse over will update style.
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