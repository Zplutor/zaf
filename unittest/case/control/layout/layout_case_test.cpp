#include <gtest/gtest.h>
#include <zaf/control/linear_box.h>
#include <zaf/creation.h>

//Children would not be layouted if it is invisible.
TEST(LayoutCaseTest, Case1) {

    auto parent = zaf::Create<zaf::VerticalBox>();
    parent->SetFixedSize(zaf::Size{ 100, 100 });

    auto child1 = zaf::Create<zaf::Control>();
    child1->SetIsVisible(false);

    auto child2 = zaf::Create<zaf::Control>();
    parent->AddChildren({ child1, child2 });

    ASSERT_EQ(child1->GetHeight(), 0);
    ASSERT_EQ(child2->GetHeight(), 100);
}


//Children can be layouted even if parent is invisible.
TEST(LayoutCaseTest, Case2) {

    auto parent = zaf::Create<zaf::VerticalBox>();
    parent->SetIsVisible(false);
    parent->SetFixedSize(zaf::Size{ 100, 100 });

    auto child = zaf::Create<zaf::Control>();
    parent->AddChild(child);

    ASSERT_EQ(child->GetWidth(), 100);
    ASSERT_EQ(child->GetHeight(), 100);
}