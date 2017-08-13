#include <gtest/gtest.h>
#include <zaf/control/button.h>
#include <zaf/control/label.h>
#include <zaf/control/split_control.h>
#include <zaf/serialization/deserializing.h>

TEST(SplitControl, Deserialize) {

    auto json = "{"
        "\"IsHorizontalSplit\": true,"
        "\"SplitBarThickness\": 5,"
        "\"SplitBarDistance\": 10,"
        "\"MinimumSplitBarDistance\": 7,"
        "\"MaximumSplitBarDistance\": 70,"
        "\"IsSplitBarDistanceFlipped\": true,"
        "\"Name\":\"split\","
        "\"Rect\": { \"Size\": { \"Width\": 100, \"Height\": 100 }},"
        "\"SplitBar\": { \"Name\": \"bar\" },"
        "\"FirstPane\": { \"Type\": \"Label\", \"Text\": \"first\" },"
        "\"SecondPane\": { \"Type\": \"Button\", \"Text\": \"second\" },"
    "}";
    auto split_control = zaf::DeserializeObjectFromJson<zaf::SplitControl>(json);
    ASSERT_EQ(split_control->GetName(), L"split");
    ASSERT_EQ(split_control->IsHorizontalSplit(), true);
    ASSERT_EQ(split_control->GetSplitBarThickness(), 5);
    ASSERT_EQ(split_control->GetSplitBarDistance(), 10);
    ASSERT_EQ(split_control->GetMinimumSplitBarDistance(), 7);
    ASSERT_EQ(split_control->GetMaximumSplitBarDistance(), 70);
    ASSERT_EQ(split_control->IsSplitBarDistanceFlipped(), true);
    ASSERT_EQ(split_control->GetSplitBar()->GetName(), L"bar");

    auto first_pane = std::dynamic_pointer_cast<zaf::Label>(split_control->GetFirstPane());
    ASSERT_NE(first_pane, nullptr);
    ASSERT_EQ(first_pane->GetText(), L"first");

    auto second_pane = std::dynamic_pointer_cast<zaf::Button>(split_control->GetSecondPane());
    ASSERT_NE(second_pane, nullptr);
    ASSERT_EQ(second_pane->GetText(), L"second");
}


TEST(SplitControlSplitBar, Deserialize) {

    auto json = "{ "
        "\"Type\": \"SplitBar.SplitControl\", "
        "\"Name\": \"splitter\"" 
    "}";
    auto split_bar = zaf::DeserializeObjectFromJson<zaf::SplitControl::SplitBar>(json);
    ASSERT_NE(split_bar, nullptr);
    ASSERT_EQ(split_bar->GetName(), L"splitter");
}


TEST(SplitControlSplitBar, DeserializeSplitterColor) {

    auto json = "{\"SplitterColor\": { \"B\": 0.83 }}";
    auto split_bar = zaf::DeserializeObjectFromJson<zaf::SplitControl::SplitBar>(json);
    ASSERT_EQ(split_bar->GetSplitterColor(), zaf::Color(0.f, 0.f, 0.83f));
}


TEST(SplitControlSplitBar, DeserializeSplitterColorPicker) {

    auto json = "{\"SplitterColorPicker\": { \"Color\": { \"B\": 0.84 }}}";
    auto split_bar = zaf::DeserializeObjectFromJson<zaf::SplitControl::SplitBar>(json);
    ASSERT_EQ(split_bar->GetSplitterColor(), zaf::Color(0.f, 0.f, 0.84f));
}