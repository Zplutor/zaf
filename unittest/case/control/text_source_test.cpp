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
    auto text = text_source->GetText();
    auto view = std::get_if<0>(&text);
    ASSERT_NE(view, nullptr);
    ASSERT_EQ(*view, L"");

    constexpr const wchar_t* TestText = L"This is a text";
    text_source->SetText(TestText);
    text = text_source->GetText();
    view = std::get_if<0>(&text);
    ASSERT_NE(view, nullptr);
    ASSERT_EQ(*view, TestText);
}


TEST(TextSourceTest, GetTextLength) {

    auto text_source = zaf::TextSource::Default();
    ASSERT_EQ(text_source->GetTextLength(), 0);

    std::wstring_view text{ L"Get text length test" };
    text_source->SetText(std::wstring{ text });
    ASSERT_EQ(text_source->GetTextLength(), text.length());
}