#include <gtest/gtest.h>
#include <zaf/creation.h>
#include <zaf/control/textual_control.h>

TEST(TextualControlTest, AutoResize) {

	auto control = zaf::Create<zaf::TextualControl>();
	control->SetAutoResize(true);
	control->SetText(L"123");

	ASSERT_NE(control->GetWidth(), 0);
	ASSERT_NE(control->GetHeight(), 0);
	ASSERT_EQ(control->GetMaximumWidth(), control->GetWidth());
	ASSERT_EQ(control->GetMaximumHeight(), control->GetHeight());
	ASSERT_EQ(control->GetMinimumWidth(), control->GetWidth());
	ASSERT_EQ(control->GetMinimumHeight(), control->GetHeight());
}