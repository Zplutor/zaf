#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/creation.h>

TEST(ControlTest, GetPreferredSize) {

    auto parent = zaf::Create<zaf::Control>();

    auto child1 = zaf::Create<zaf::Control>();
    auto child2 = zaf::Create<zaf::Control>();
    parent->AddChildren({ child1, child2 });

    //Children have intersected rect.
    child1->SetRect(zaf::Rect(0, 0, 10, 10));
    child2->SetRect(zaf::Rect(5, 5, 10, 10));
    ASSERT_EQ(parent->GetPreferredSize(), zaf::Size(15, 15));

    //Chilren are attach to each other.
    child1->SetRect(zaf::Rect(0, 0, 10, 10));
    child2->SetRect(zaf::Rect(0, 10, 10, 10));
    ASSERT_EQ(parent->GetPreferredSize(), zaf::Size(10, 20));

    //Children don't have intersected rect, and are not attach to each other.
    child1->SetRect(zaf::Rect(0, 0, 10, 10));
    child2->SetRect(zaf::Rect(50, 50, 5, 5));
    ASSERT_EQ(parent->GetPreferredSize(), zaf::Size(55, 55));

    //Children have negative position.
    child1->SetRect(zaf::Rect(-5, -5, 10, 10));
    child2->SetRect(zaf::Rect(-30, -30, 5, 5));
    ASSERT_EQ(parent->GetPreferredSize(), zaf::Size(5, 5));

    //Children have margins.
    child1->SetRect(zaf::Rect(0, 0, 5, 5));
    child1->SetMargin(2);
    child2->SetRect(zaf::Rect(0, 5, 5, 5));
    child2->SetMargin(1);
    ASSERT_EQ(parent->GetPreferredSize(), zaf::Size(7, 11));

    //Parent has border and padding.
    parent->SetBorder(3);
    parent->SetPadding(4);
    ASSERT_EQ(parent->GetPreferredSize(), zaf::Size(21, 25));
}


TEST(ControlTest, AutoResize) {

    auto child = zaf::Create<zaf::Control>();
    child->SetSize(zaf::Size{ 40, 60 });

    auto parent = zaf::Create<zaf::Control>();
    parent->SetAutoResize(true);

    //Children changed.
    parent->AddChild(child);
    ASSERT_EQ(parent->GetSize(), child->GetSize());

    //Children rect changed.
    child->SetRect(zaf::Rect{ 10, 10, 50, 50 });
    ASSERT_EQ(parent->GetSize(), zaf::Size(60, 60));

    //Children margin changed.
    child->SetMargin(zaf::Frame{ 2, 2, 2, 2 });
    ASSERT_EQ(parent->GetSize(), zaf::Size(62, 62));

    //Children visiblity chagned.
    child->SetIsVisible(false);
    ASSERT_EQ(parent->GetSize(), zaf::Size(0, 0));
}