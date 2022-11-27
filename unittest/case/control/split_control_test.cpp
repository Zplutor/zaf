#include <gtest/gtest.h>
#include <zaf/control/split_control.h>
#include <zaf/creation.h>

TEST(SplitControlTest, SetSplitBarThickness) {

    auto split_control = zaf::Create<zaf::SplitControl>();
    split_control->SetSize(zaf::Size{ 10, 10});

    split_control->SetSplitBarThickness(2);
    ASSERT_EQ(split_control->SplitDistance(), 4);
}


TEST(SplitControlTest, AutoSplitDistance) {

    auto split_control = zaf::Create<zaf::SplitControl>();

    split_control->SetSplitBarThickness(4);
    ASSERT_EQ(split_control->SplitDistance(), 0);

    split_control->SetWidth(100);
    ASSERT_EQ(split_control->SplitDistance(), 48);

    split_control->SetWidth(50);
    ASSERT_EQ(split_control->SplitDistance(), 23);
}


TEST(SplitControlTest, SetSplitDistance) {

    auto split_control = zaf::Create<zaf::SplitControl>();
    split_control->SetSplitBarThickness(4);
    split_control->SetWidth(50);

    split_control->SetSplitDistance(0);
    ASSERT_EQ(split_control->SplitDistance(), 0);

    split_control->SetSplitDistance(10);
    ASSERT_EQ(split_control->SplitDistance(), 10);

    split_control->SetSplitDistance(25);
    ASSERT_EQ(split_control->SplitDistance(), 25);

    split_control->SetSplitDistance(60);
    ASSERT_EQ(split_control->SplitDistance(), 46);
}


TEST(SplitControlTest, SetSplitDistanceWithFirstPaneMinMax) {

    auto split_control = zaf::Create<zaf::SplitControl>();
    split_control->SetSplitBarThickness(4);
    split_control->SetSize(zaf::Size{ 50, 50 });
    split_control->FirstPane()->SetMargin(zaf::Frame{ 2, 2, 2, 2 });
    split_control->SecondPane()->SetMargin(zaf::Frame{ 4, 4, 4, 4 });

    split_control->SetFirstPaneMinLength(10);
    split_control->SetFirstPaneMaxLength(40);

    split_control->SetSplitDistance(5);
    ASSERT_EQ(split_control->SplitDistance(), 10);
    ASSERT_EQ(split_control->FirstPane()->Rect(), zaf::Rect(2, 2, 6, 46 ));
    ASSERT_EQ(split_control->SplitBar()->Rect(), zaf::Rect(10, 0, 4, 50));
    ASSERT_EQ(split_control->SecondPane()->Rect(), zaf::Rect(18, 4, 28, 42));

    split_control->SetSplitDistance(20);
    ASSERT_EQ(split_control->SplitDistance(), 20);
    ASSERT_EQ(split_control->FirstPane()->Rect(), zaf::Rect(2, 2, 16, 46));
    ASSERT_EQ(split_control->SplitBar()->Rect(), zaf::Rect(20, 0, 4, 50));
    ASSERT_EQ(split_control->SecondPane()->Rect(), zaf::Rect(28, 4, 18, 42));

    split_control->SetSplitDistance(45);
    ASSERT_EQ(split_control->SplitDistance(), 40);
    ASSERT_EQ(split_control->FirstPane()->Rect(), zaf::Rect(2, 2, 36, 46));
    ASSERT_EQ(split_control->SplitBar()->Rect(), zaf::Rect(40, 0, 4, 50));
    ASSERT_EQ(split_control->SecondPane()->Rect(), zaf::Rect(48, 4, 0, 42));
}


TEST(SplitControlTest, SetSplitDistanceWithSecondPaneMinMax) {

    auto split_control = zaf::Create<zaf::SplitControl>();
    split_control->SetSplitBarThickness(4);
    split_control->SetSize(zaf::Size{ 50, 50 });
    split_control->FirstPane()->SetMargin(zaf::Frame{ 2, 2, 2, 2 });
    split_control->SecondPane()->SetMargin(zaf::Frame{ 2, 2, 2, 2 });

    split_control->SetSecondPaneMinLength(10);
    split_control->SetSecondPaneMaxLength(40);

    split_control->SetSplitDistance(5);
    ASSERT_EQ(split_control->SplitDistance(), 6);
    ASSERT_EQ(split_control->FirstPane()->Rect(), zaf::Rect(2, 2, 2, 46));
    ASSERT_EQ(split_control->SplitBar()->Rect(), zaf::Rect(6, 0, 4, 50));
    ASSERT_EQ(split_control->SecondPane()->Rect(), zaf::Rect(12, 2, 36, 46));

    split_control->SetSplitDistance(20);
    ASSERT_EQ(split_control->SplitDistance(), 20);
    ASSERT_EQ(split_control->FirstPane()->Rect(), zaf::Rect(2, 2, 16, 46));
    ASSERT_EQ(split_control->SplitBar()->Rect(), zaf::Rect(20, 0, 4, 50));
    ASSERT_EQ(split_control->SecondPane()->Rect(), zaf::Rect(26, 2, 22, 46));

    split_control->SetSplitDistance(50);
    ASSERT_EQ(split_control->SplitDistance(), 36);
    ASSERT_EQ(split_control->FirstPane()->Rect(), zaf::Rect(2, 2, 32, 46));
    ASSERT_EQ(split_control->SplitBar()->Rect(), zaf::Rect(36, 0, 4, 50));
    ASSERT_EQ(split_control->SecondPane()->Rect(), zaf::Rect(42, 2, 6, 46));
}


TEST(SplitControlTest, SetSplitDistanceWithPanesMinMax) {

    auto split_control = zaf::Create<zaf::SplitControl>();
    split_control->SetSplitBarThickness(4);
    split_control->SetSize(zaf::Size{ 50, 50 });
    split_control->FirstPane()->SetMargin(zaf::Frame{ 2, 2, 2, 2 });
    split_control->SecondPane()->SetMargin(zaf::Frame{ 2, 2, 2, 2 });

    split_control->SetFirstPaneMaxLength(20);
    split_control->SetSecondPaneMaxLength(20);

    split_control->SetSplitDistance(25);
    ASSERT_EQ(split_control->SplitDistance(), 20);
}