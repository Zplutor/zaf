#include <gtest/gtest.h>
#include <zaf/creation.h>
#include <zaf/control/textual_control.h>

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