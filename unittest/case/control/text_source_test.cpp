#include <gtest/gtest.h>
#include <zaf/control/text_source.h>

TEST(TextSourceTest, SetText) {

    auto text_source = zaf::TextSource::Default();
    auto result = text_source->SetText(L"This is a text");
    ASSERT_EQ(result.IsChanged(), true);
    ASSERT_EQ(result.IsNotificationSent(), false);

    result = text_source->SetText(L"This is a text");
    ASSERT_EQ(result.IsChanged(), false);
    ASSERT_EQ(result.IsNotificationSent(), false);
}


TEST(TextSourceTest, GetText) {

    auto text_source = zaf::TextSource::Default();
    text_source->SetText(L"This is a text");

    auto text = text_source->GetText();
    auto view = std::get_if<0>(&text);
    ASSERT_NE(view, nullptr);
}