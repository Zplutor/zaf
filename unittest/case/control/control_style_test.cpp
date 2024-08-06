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
        leaf1_->SetCanFocused(true);
        leaf1_->SetRect(Rect{ 0, 0, 100, 100 });

        leaf2_ = Create<Control>();
        leaf2_->SetCanFocused(true);
        leaf2_->SetRect(Rect{ 0, 0, 100, 100 });

        stem_ = Create<Control>();
        stem_->SetCanFocused(true);
        stem_->AddChildren({ leaf1_, leaf2_ });
        stem_->SetRect(Rect{ 0, 0, 100, 100 });

        test_window_->RootControl()->AddChild(stem_);

        //Consume any repaint requirement.
        RepaintWindow();

        //Move leaf2 outside the visible area. This should be done after consuming the repaint
        //requirement in order to reset its style update requirement.
        leaf2_->SetPosition(Point{ 1000, 0 });

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

    static const std::shared_ptr<Window>& TestWindow() {
        return test_window_;
    }

    const std::shared_ptr<Control>& Stem() const {
        return stem_;
    }

    const std::shared_ptr<Control>& Leaf1() const {
        return leaf1_;
    }

    const std::shared_ptr<Control>& Leaf2() const {
        return leaf2_;
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
    // Leaf2 won't update as it is outside the visible area.
    ASSERT_EQ(Leaf2UpdateCount(), 0); 
}


TEST_F(ControlStyleTest, UpdateOnIsSelectedChange) {

    Stem()->SetIsSelected(true);
    RepaintWindow();

    ASSERT_EQ(RootUpdateCount(), 0);
    ASSERT_EQ(StemUpdateCount(), 1);
    ASSERT_EQ(Leaf1UpdateCount(), 1);
    // Leaf2 won't update as it is outside the visible area.
    ASSERT_EQ(Leaf2UpdateCount(), 0); 
}


TEST_F(ControlStyleTest, UpdateOnIsVisibleChange) {

    //Change visibility to false won't trigger update.
    Stem()->SetIsVisible(false);
    RepaintWindow();
    ASSERT_EQ(RootUpdateCount(), 0);
    ASSERT_EQ(StemUpdateCount(), 0);
    ASSERT_EQ(Leaf1UpdateCount(), 0);
    ASSERT_EQ(Leaf2UpdateCount(), 0);

    Stem()->SetIsVisible(true);
    RepaintWindow();
    ASSERT_EQ(RootUpdateCount(), 0);
    ASSERT_EQ(StemUpdateCount(), 1);
    ASSERT_EQ(Leaf1UpdateCount(), 1);
    // Leaf2 won't update as it is outside the visible area.
    ASSERT_EQ(Leaf2UpdateCount(), 0);
}


TEST_F(ControlStyleTest, UpdateOnIsFocusedChange) {

    Stem()->SetIsFocused(true);
    RepaintWindow();

    ASSERT_EQ(RootUpdateCount(), 1);
    ASSERT_EQ(StemUpdateCount(), 1);
    ASSERT_EQ(Leaf1UpdateCount(), 1);
    // Leaf2 won't update as it is outside the visible area.
    ASSERT_EQ(Leaf2UpdateCount(), 0);
}


TEST_F(ControlStyleTest, UpdateOnIsMouseOverChange) {

    TestWindow()->Messager().Send(WM_MOUSEMOVE, 0, 0);
    RepaintWindow();

    ASSERT_EQ(RootUpdateCount(), 1);
    ASSERT_EQ(StemUpdateCount(), 1);
    ASSERT_EQ(Leaf1UpdateCount(), 1);
    // Leaf2 won't update as it is outside the visible area.
    ASSERT_EQ(Leaf2UpdateCount(), 0);
}


TEST_F(ControlStyleTest, UpdateOnPositionChange) {

    //At first, the control is outside the visible area, it won't update style.
    //Once it becomes visible, it should update style.
    Stem()->SetIsEnabled(false);
    RepaintWindow();
    ASSERT_EQ(RootUpdateCount(), 0);
    ASSERT_EQ(StemUpdateCount(), 1);
    ASSERT_EQ(Leaf1UpdateCount(), 1);
    ASSERT_EQ(Leaf2UpdateCount(), 0);

    Leaf2()->SetPosition(Point{});
    RepaintWindow();
    ASSERT_EQ(RootUpdateCount(), 0);
    ASSERT_EQ(StemUpdateCount(), 1);
    ASSERT_EQ(Leaf1UpdateCount(), 1);
    ASSERT_EQ(Leaf2UpdateCount(), 1);
}


TEST_F(ControlStyleTest, NoExtraPaintWhenUpdatingStyle) {

    auto sub = Leaf1()->StyleUpdateEvent().Subscribe([](const StyleUpdateInfo& event_info) {
        auto control = As<Control>(event_info.Source());
        //Imminate a property setting that will repaint the control.
        control->NeedRepaint();
    });

    int paint_count{};
    auto window_sub = TestWindow()->MessageReceivedEvent().Subscribe(
        [&paint_count](const MessageReceivedInfo& event_info) {
        if (event_info.Message().ID() == WM_PAINT) {
            paint_count++;
        }
    });

    //This will update style.
    Leaf1()->SetIsEnabled(false);

    //Trigger the WM_PAINT mandatorily.
    RepaintWindow();

    ASSERT_EQ(paint_count, 1);

    BOOL has_update_rect = GetUpdateRect(TestWindow()->Handle(), nullptr, FALSE);
    ASSERT_FALSE(has_update_rect);
}