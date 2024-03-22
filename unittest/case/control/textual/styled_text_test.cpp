#include <gtest/gtest.h>
#include <zaf/control/textual/styled_text.h>

using namespace zaf;
using namespace zaf::textual;

static_assert(!std::is_copy_constructible_v<StyledText>);
static_assert(!std::is_copy_assignable_v<StyledText>);
static_assert(std::is_move_constructible_v<StyledText>);
static_assert(std::is_move_assignable_v<StyledText>);

TEST(StyledTextTest, SetTextInRange) {

    StyledText styled_text(L"StyledText");
    auto new_range = styled_text.SetTextInRange(L"ED-", Range{ 4, 2 });
    ASSERT_EQ(new_range, Range(4, 3));
    ASSERT_EQ(styled_text.Text(), L"StylED-Text");
}


TEST(StyledTextTest, AppendText) {

    StyledText styled_text(L"StyledText");
    auto new_range = styled_text.AppendText(L"-append");
    ASSERT_EQ(new_range, Range(10, 7));
    ASSERT_EQ(styled_text.Text(), L"StyledText-append");
}