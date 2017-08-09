#include <gtest/gtest.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/serialization/deserializing.h>

TEST(ScrollBar, Deserialize) {

    auto json = "{ "
        "\"IsHorizontal\": true, "
        "\"ArrowLength\": 40, "
        "\"MinimumValue\": -11, "
        "\"MaximumValue\": 11, "
        "\"Value\": 7,"
        "\"SmallChangeValue\": 3, "
        "\"LargeChangeValue\": 33,"
        "\"Name\": \"scrollbar\","
        "\"IncrementalArrow\": { \"Name\": \"incremental\" }"
        "\"DecrementalArrow\": { \"Name\": \"decremental\" }"
        "\"Thumb\": { \"Name\":\"thumb\" }"
    "}";
    auto scroll_bar = zaf::DeserializeObjectFromJson<zaf::ScrollBar>(json);
    ASSERT_TRUE(scroll_bar->IsHorizontal());
    ASSERT_EQ(scroll_bar->GetArrowLength(), 40);
    ASSERT_EQ(scroll_bar->GetMinimumValue(), -11);
    ASSERT_EQ(scroll_bar->GetMaximumValue(), 11);
    ASSERT_EQ(scroll_bar->GetValue(), 7);
    ASSERT_EQ(scroll_bar->GetSmallChangeValue(), 3);
    ASSERT_EQ(scroll_bar->GetLargeChangeValue(), 33);
    ASSERT_EQ(scroll_bar->GetName(), L"scrollbar");
    ASSERT_EQ(scroll_bar->GetIncrementalArrow()->GetName(), L"incremental");
    ASSERT_EQ(scroll_bar->GetDecrementalArrow()->GetName(), L"decremental");
    ASSERT_EQ(scroll_bar->GetThumb()->GetName(), L"thumb");
}


TEST(ScrollBarArrow, Deserialize) {

    auto json = "{ \"ArrowColor\": { \"R\": 0.43 } }";
    auto arrow = zaf::DeserializeObjectFromJson<zaf::ScrollBar::Arrow>(json);
    ASSERT_EQ(arrow->GetArrowColor(), zaf::Color(0.43f, 0, 0));

    json = "{ \"ArrowColorPicker\": { \"Color\": { \"R\": 0.45 } } }";
    arrow = zaf::DeserializeObjectFromJson<zaf::ScrollBar::Arrow>(json);
    ASSERT_EQ(arrow->GetArrowColor(), zaf::Color(0.45f, 0, 0));
}


TEST(ScrollBarThumb, Deserialize) {

    auto json = "{ \"ThumbColor\": { \"R\": 0.33 } }";
    auto thumb = zaf::DeserializeObjectFromJson<zaf::ScrollBar::Thumb>(json);
    ASSERT_EQ(thumb->GetThumbColor(), zaf::Color(0.33f, 0, 0));

    json = "{ \"ThumbColorPicker\": { \"Color\": { \"R\": 0.35 } } }";
    thumb = zaf::DeserializeObjectFromJson<zaf::ScrollBar::Thumb>(json);
    ASSERT_EQ(thumb->GetThumbColor(), zaf::Color(0.35f, 0, 0));
}