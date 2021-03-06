#include <gtest/gtest.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/control/linear_box.h>
#include <zaf/creation.h>
#include <zaf/parsing/helpers.h>

TEST(LinearBoxTest, Normal) {

    auto vertical_box = zaf::Create<zaf::VerticalBox>();
    auto vertical_layouter = std::dynamic_pointer_cast<zaf::VerticalLayouter>(
        vertical_box->GetLayouter());

    ASSERT_NE(vertical_layouter, nullptr);
    ASSERT_EQ(vertical_layouter->GetDirection(), zaf::LayoutDirection::TopToBottom);

    auto horizontal_box = zaf::Create<zaf::HorizontalBox>();
    auto horizontal_layouter = std::dynamic_pointer_cast<zaf::HorizontalLayouter>(
        horizontal_box->GetLayouter());

    ASSERT_NE(horizontal_layouter, nullptr);
    ASSERT_EQ(horizontal_layouter->GetDirection(), zaf::LayoutDirection::LeftToRight);
}


TEST(LinearBoxTest, Parse) {

    std::wstring xaml = LR"(<VerticalBox />)";
    auto vertical_box = zaf::CreateObjectFromXaml<zaf::VerticalBox>(xaml);
    ASSERT_NE(vertical_box, nullptr);

    xaml = LR"(<HorizontalBox />)";
    auto horizontal_box = zaf::CreateObjectFromXaml<zaf::HorizontalBox>(xaml);
    ASSERT_NE(horizontal_box, nullptr);
}