#include <gtest/gtest.h>
#include <zaf/control/textual_control.h>
#include "utility.h"

namespace {

std::shared_ptr<zaf::TextualControl> CreateTextualControlFromXaml(const std::string& xaml) {
    return CreateObjectFromXaml<zaf::TextualControl>(xaml);
}


TEST(TextualControlParser, ParseText) {

    auto xaml = R"(<TextualControl text="ttttt"></TextualControl>)";
    auto control = CreateTextualControlFromXaml(xaml);
    ASSERT_EQ(control->GetText(), L"ttttt");

    xaml = R"(
        <TextualControl>
            <TextualControl.Text>eeeeeee</TextualControl.Text>
        </TextualControl>
    )";
    control = CreateTextualControlFromXaml(xaml);
    ASSERT_EQ(control->GetText(), L"eeeeeee");
}


TEST(TextualControlParser, ParseDefaultTextColor) {

    auto xaml = R"(<TextualControl defaultTextColor="#aabbcc" />)";
    auto control = CreateTextualControlFromXaml(xaml);
    ASSERT_EQ(control->GetDefaultTextColor(), zaf::Color::FromRGB(0xaabbcc));

    xaml = R"(
        <TextualControl>
            <TextualControl.DefaultTextColor r="0.3" g="0.4" b="0.5" />
        </TextualControl>
    )";
    control = CreateTextualControlFromXaml(xaml);
    ASSERT_EQ(control->GetDefaultTextColor(), zaf::Color(0.3f, 0.4f, 0.5f));
}

}
