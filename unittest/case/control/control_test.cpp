#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>

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