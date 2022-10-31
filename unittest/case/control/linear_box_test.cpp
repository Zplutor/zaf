#include <gtest/gtest.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/control/linear_box.h>
#include <zaf/creation.h>
#include <zaf/object/parsing/helpers.h>

TEST(LinearBoxTest, Normal) {

    auto vertical_box = zaf::Create<zaf::VerticalBox>();
    auto vertical_layouter = std::dynamic_pointer_cast<zaf::VerticalLayouter>(
        vertical_box->Layouter());

    ASSERT_NE(vertical_layouter, nullptr);
    ASSERT_EQ(vertical_layouter->Direction(), zaf::LayoutDirection::TopToBottom);

    auto horizontal_box = zaf::Create<zaf::HorizontalBox>();
    auto horizontal_layouter = std::dynamic_pointer_cast<zaf::HorizontalLayouter>(
        horizontal_box->Layouter());

    ASSERT_NE(horizontal_layouter, nullptr);
    ASSERT_EQ(horizontal_layouter->Direction(), zaf::LayoutDirection::LeftToRight);
}


TEST(LinearBoxTest, Parse) {

    std::wstring xaml = LR"(<VerticalBox AxisAlignment="Start" CrossAxisAlignment="End" />)";
    auto vertical_box = zaf::CreateObjectFromXaml<zaf::VerticalBox>(xaml);
    ASSERT_NE(vertical_box, nullptr);
    ASSERT_EQ(vertical_box->Direction(), zaf::LayoutDirection::TopToBottom);
    ASSERT_EQ(vertical_box->AxisAlignment(), zaf::AxisAlignment::Start);
    ASSERT_EQ(vertical_box->CrossAxisAlignment(), zaf::AxisAlignment::End);

    xaml = LR"(<HorizontalBox AxisAlignment="End" CrossAxisAlignment="Start" />)";
    auto horizontal_box = zaf::CreateObjectFromXaml<zaf::HorizontalBox>(xaml);
    ASSERT_NE(horizontal_box, nullptr);
    ASSERT_EQ(horizontal_box->Direction(), zaf::LayoutDirection::LeftToRight);
    ASSERT_EQ(horizontal_box->AxisAlignment(), zaf::AxisAlignment::End);
    ASSERT_EQ(horizontal_box->CrossAxisAlignment(), zaf::AxisAlignment::Start);
}