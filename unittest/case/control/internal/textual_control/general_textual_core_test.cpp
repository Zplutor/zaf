#include <gtest/gtest.h>
#include <zaf/control/internal/textual_control/general_textual_core.h>

TEST(GeneralTextualCoreTest, SetText) {

    zaf::internal::GeneralTextualCore core;

    auto result = core.SetText(L"This is a text");
    ASSERT_EQ(result.is_changed, true);
    ASSERT_EQ(result.is_notification_sent, false);

    result = core.SetText(L"This is a text");
    ASSERT_EQ(result.is_changed, false);
    ASSERT_EQ(result.is_notification_sent, false);
}


TEST(GeneralTextualCoreTest, GetText) {

    zaf::internal::GeneralTextualCore core;

    auto text = core.GetText();
    auto view = std::get_if<0>(&text);
    ASSERT_NE(view, nullptr);
    ASSERT_EQ(*view, L"");

    constexpr const wchar_t* TestText = L"This is a text";
    core.SetText(TestText);
    text = core.GetText();
    view = std::get_if<0>(&text);
    ASSERT_NE(view, nullptr);
    ASSERT_EQ(*view, TestText);
}


TEST(GeneralTextualCoreTest, GetTextLength) {

    zaf::internal::GeneralTextualCore core;

    ASSERT_EQ(core.GetTextLength(), 0);

    std::wstring_view text{ L"Get text length test" };
    core.SetText(std::wstring{ text });
    ASSERT_EQ(core.GetTextLength(), text.length());
}