#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/control/layout/stretch_layouter.h>
#include <zaf/creation.h>

TEST(StretchLayouterTest, Layout) {

    auto parent = zaf::Create<zaf::Control>();
    parent->SetSize(zaf::Size(100, 50));
    parent->SetPadding(zaf::Frame(1));
    parent->SetBorder(zaf::Frame(3));

    auto child1 = zaf::Create<zaf::Control>();
    auto child2 = zaf::Create<zaf::Control>();
    parent->AddChildren({ child1, child2 });

    parent->SetLayouter(zaf::Create<zaf::StretchLayouter>());

    ASSERT_EQ(child1->Rect(), zaf::Rect(0, 0, 92, 42));
    ASSERT_EQ(child2->Rect(), zaf::Rect(0, 0, 92, 42));
}