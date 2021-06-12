#include <gtest/gtest.h>
#include <zaf/creation.h>
#include <zaf/control/textual_control.h>

TEST(TextualControlTest, AutoResize) {

	auto control = zaf::Create<zaf::TextualControl>();
	control->SetAutoResize(true);
	control->SetText(L"123");

	ASSERT_NE(control->GetWidth(), 0);
	ASSERT_NE(control->GetHeight(), 0);
	ASSERT_EQ(control->GetMaxWidth(), control->GetWidth());
	ASSERT_EQ(control->GetMaxHeight(), control->GetHeight());
	ASSERT_EQ(control->GetMinWidth(), control->GetWidth());
	ASSERT_EQ(control->GetMinHeight(), control->GetHeight());
}