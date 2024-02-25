#include <optional>
#include <gtest/gtest.h>
#include <zaf/creation.h>
#include <zaf/control/textual_control.h>

TEST(TextualControlTest, SetText) {

    auto control = zaf::Create<zaf::TextualControl>();

    bool is_event_raised{};
    auto sub = control->TextChangedEvent().Subscribe(
        [&is_event_raised](const zaf::TextChangedInfo& event_info) {
        is_event_raised = true;
    });

    control->SetText(L"textual");
    ASSERT_EQ(control->Text(), L"textual");
    ASSERT_TRUE(is_event_raised);

    //The event will be raised even if the text is the same.
    is_event_raised = false;
    control->SetText(L"textual");
    ASSERT_EQ(control->Text(), L"textual");
    ASSERT_TRUE(is_event_raised);
}


TEST(TextualControlTest, SetTextInRange) {

    auto control = zaf::Create<zaf::TextualControl>();

    bool is_event_raised{};
    auto sub = control->TextChangedEvent().Subscribe(
        [&is_event_raised](const zaf::TextChangedInfo& event_info) {
        is_event_raised = true;
    });

    control->SetTextInRange(L"textual", zaf::Range{ 0, 0 });
    ASSERT_EQ(control->Text(), L"textual");
    ASSERT_TRUE(is_event_raised);

    //The event will be raised even if the text is the same.
    is_event_raised = false;
    control->SetTextInRange(L"textual", zaf::Range{ 0, control->TextLength() });
    ASSERT_EQ(control->Text(), L"textual");
    ASSERT_TRUE(is_event_raised);

    //Set sub range text
    is_event_raised = false;
    control->SetTextInRange(L"EXT", zaf::Range{ 1, 2 });
    ASSERT_EQ(control->Text(), L"tEXTtual");
    ASSERT_TRUE(is_event_raised);

    //Invalid range.
    ASSERT_THROW(control->SetTextInRange(L"abc", zaf::Range{ 8, 2 }), std::logic_error);
    ASSERT_THROW(control->SetTextInRange(L"abc", zaf::Range{ 9, 1 }), std::logic_error);
}


TEST(TextualControlTest, AutoSize) {

    auto control = zaf::Create<zaf::TextualControl>();
    control->SetAutoSize(true);

    //Auto size after changing text.
    control->SetText(L"123");
    ASSERT_NE(control->Width(), 0);
    ASSERT_NE(control->Height(), 0);
    ASSERT_EQ(control->MaxWidth(), control->Width());
    ASSERT_EQ(control->MaxHeight(), control->Height());
    ASSERT_EQ(control->MinWidth(), control->Width());
    ASSERT_EQ(control->MinHeight(), control->Height());

    //Auto size after changing font.
    auto old_size = control->Size();
    control->SetFont(zaf::Font{ L"", 100 });
    ASSERT_NE(control->Size(), old_size);
}


TEST(TextualControlTest, AutoSizeOnWordWrappingChanged) {

    auto control = zaf::Create<zaf::TextualControl>();
    control->SetText(L"01234567890123456789");
    control->SetWidth(10);

    //After setting auto height, the fixed height will be set to single line height.
    control->SetAutoHeight(true);
    auto old_height = control->Height();

    //After setting word wrapping, the fixed height should be set to multi-line height.
    control->SetWordWrapping(zaf::WordWrapping::Character);
    ASSERT_NE(control->Height(), old_height);
    ASSERT_EQ(control->MinHeight(), control->Height());
    ASSERT_EQ(control->MaxHeight(), control->Height());
}


TEST(TextualControlTest, AutoSizeOnSizeChanged) {

    auto control = zaf::Create<zaf::TextualControl>();
    control->SetText(L"01234567890123456789");
    control->SetWordWrapping(zaf::WordWrapping::Character);
    control->SetWidth(10);
    control->SetAutoHeight(true);

    std::size_t event_call_times{};
    zaf::Size event_previous_size{};
    auto sub = control->SizeChangedEvent().Subscribe([&](const zaf::SizeChangedInfo& event_info) {
        ++event_call_times;
        event_previous_size = event_info.PreviousSize();
    });

    //Setting the same width shouldn't change the size.
    control->SetWidth(10);
    ASSERT_EQ(event_call_times, 0);

    //After changing the width, the height should be changed as well.
    auto old_height = control->Height();
    control->SetWidth(20);

    //The height in the event should be the old height.
    ASSERT_EQ(event_previous_size.height, old_height);
    //SizeChanged event should be raised once only.
    ASSERT_EQ(event_call_times, 1);

    ASSERT_EQ(control->Width(), 20);
    ASSERT_NE(old_height, control->Height());
    ASSERT_EQ(control->MinHeight(), control->Height());
    ASSERT_EQ(control->MaxHeight(), control->Height());
}


TEST(TextualControlTest, SetFont) {

    auto control = zaf::Create<zaf::TextualControl>();
    control->SetText(L"012345");

    zaf::Font font_in_range;
    font_in_range.size = 50;
    control->SetFontInRange(font_in_range, zaf::Range{ 1, 3 });

    zaf::Font default_font;
    default_font.size = 30;
    control->SetFont(default_font);
    ASSERT_EQ(control->Font(), default_font);

    //Ranged fonts will be clear after SetFont().
    ASSERT_EQ(control->GetFontAtIndex(0), default_font);
    ASSERT_EQ(control->GetFontAtIndex(1), default_font);
    ASSERT_EQ(control->GetFontAtIndex(2), default_font);
    ASSERT_EQ(control->GetFontAtIndex(3), default_font);
    ASSERT_EQ(control->GetFontAtIndex(4), default_font);
    ASSERT_EQ(control->GetFontAtIndex(5), default_font);
}


TEST(TextualControlTest, SetFontInRange) {

    auto control = zaf::Create<zaf::TextualControl>();
    control->SetText(L"12345");

    zaf::Font default_font;
    default_font.size = 30;
    control->SetFont(default_font);

    zaf::Font font;
    font.size = 50;
    control->SetFontInRange(font, zaf::Range{ 2, 2 });

    ASSERT_EQ(control->GetFontAtIndex(0), default_font);
    ASSERT_EQ(control->GetFontAtIndex(1), default_font);
    ASSERT_EQ(control->GetFontAtIndex(2), font);
    ASSERT_EQ(control->GetFontAtIndex(3), font);
    ASSERT_EQ(control->GetFontAtIndex(4), default_font);

    //Font set in range will be clear after setting a new text.
    control->SetText(L"123456");
    ASSERT_EQ(control->GetFontAtIndex(0), default_font);
    ASSERT_EQ(control->GetFontAtIndex(1), default_font);
    ASSERT_EQ(control->GetFontAtIndex(2), default_font);
    ASSERT_EQ(control->GetFontAtIndex(3), default_font);
    ASSERT_EQ(control->GetFontAtIndex(4), default_font);
    ASSERT_EQ(control->GetFontAtIndex(5), default_font);
}


TEST(TextualControlTest, SetTextColor) {

    auto control = zaf::Create<zaf::TextualControl>();
    control->SetText(L"012345");

    zaf::Color color_in_range = zaf::Color::Red();
    control->SetTextColorInRange(color_in_range, zaf::Range{ 1, 3 });

    zaf::Color default_color = zaf::Color::Black();
    control->SetTextColor(default_color);
    ASSERT_EQ(control->TextColor(), default_color);

    //Ranged text colors will be clear after SetTextColor().
    ASSERT_EQ(control->GetTextColorAtIndex(0), default_color);
    ASSERT_EQ(control->GetTextColorAtIndex(1), default_color);
    ASSERT_EQ(control->GetTextColorAtIndex(2), default_color);
    ASSERT_EQ(control->GetTextColorAtIndex(3), default_color);
    ASSERT_EQ(control->GetTextColorAtIndex(4), default_color);
    ASSERT_EQ(control->GetTextColorAtIndex(5), default_color);
}


TEST(TextualControlTest, SetTextColorInRange) {

    auto control = zaf::Create<zaf::TextualControl>();
    control->SetText(L"12345");

    zaf::Color default_color = zaf::Color::Black();
    zaf::Color new_color = zaf::Color::Red();
    control->SetTextColorInRange(new_color, zaf::Range{ 2, 2 });

    ASSERT_EQ(control->GetTextColorAtIndex(0), default_color);
    ASSERT_EQ(control->GetTextColorAtIndex(1), default_color);
    ASSERT_EQ(control->GetTextColorAtIndex(2), new_color);
    ASSERT_EQ(control->GetTextColorAtIndex(3), new_color);
    ASSERT_EQ(control->GetTextColorAtIndex(4), default_color);

    //Color set in range will be clear after setting a new text.
    control->SetText(L"123456");
    ASSERT_EQ(control->GetTextColorAtIndex(0), default_color);
    ASSERT_EQ(control->GetTextColorAtIndex(1), default_color);
    ASSERT_EQ(control->GetTextColorAtIndex(2), default_color);
    ASSERT_EQ(control->GetTextColorAtIndex(3), default_color);
    ASSERT_EQ(control->GetTextColorAtIndex(4), default_color);
    ASSERT_EQ(control->GetTextColorAtIndex(5), default_color);
}