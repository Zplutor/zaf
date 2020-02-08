#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include "utility.h"

namespace {

std::shared_ptr<zaf::Control> CreateControlFromXaml(const std::string& xaml) {
    return zaf::CreateObjectFromXaml<zaf::Control>(xaml);
}

}


TEST(ControlParser, ParseName) {

    auto xaml = R"(<Control Name="xyz" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetName(), L"xyz");

    xaml = R"(
        <Control>
            <Control.Name>abc</Control.Name>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetName(), L"abc");
}


TEST(ControlParser, ParseXAndY) {

    auto xaml = R"(<Control X="1" Y="2" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetPosition(), zaf::Point(1, 2));

    xaml = R"(
        <Control>
            <Control.X>3</Control.X>
            <Control.Y>4</Control.Y>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetPosition(), zaf::Point(3, 4));
}


TEST(ControlParser, ParsePosition) {

    auto xaml = R"(<Control Position="1,2" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetPosition(), zaf::Point(1, 2));

    xaml = R"(
        <Control>
            <Control.Position X="3" Y="4" />
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetPosition(), zaf::Point(3, 4));
}


TEST(ControlParser, ParseWidthAndHeight) {

    auto xaml = R"(<Control Width="1" Height="2" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetSize(), zaf::Size(1, 2));

    xaml = R"(
        <Control>
            <Control.Width>3</Control.Width>
            <Control.Height>4</Control.Height>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetSize(), zaf::Size(3, 4));
}


TEST(ControlParser, ParseSize) {

    auto xaml = R"(<Control Size="1,2" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetSize(), zaf::Size(1, 2));

    xaml = R"(
        <Control>
            <Control.Size Width="3" Height="4" />
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetSize(), zaf::Size(3, 4));
}


TEST(ControlParser, ParseRect) {

    auto xaml = R"(
        <Control>
            <Control.Rect Position="1,2" Size="3,4" />
        </Control>
    )";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetPosition(), zaf::Point(1, 2));
    ASSERT_EQ(control->GetSize(), zaf::Size(3, 4));
}


TEST(ControlParser, ParseMinimumWidthAndMaximumWidth) {

    auto xaml = R"(<Control MinimumWidth="1" MaximumWidth="2"></Control>)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMinimumWidth(), 1);
    ASSERT_EQ(control->GetMaximumWidth(), 2);

    xaml = R"(
        <Control>
            <Control.MinimumWidth>3</Control.MinimumWidth>
            <Control.MaximumWidth>4</Control.MaximumWidth>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMinimumWidth(), 3);
    ASSERT_EQ(control->GetMaximumWidth(), 4);
}


TEST(ControlParser, ParseMinimumHeightAndMaximumHeight) {

    auto xaml = R"(<Control MinimumHeight="1" MaximumHeight="2"></Control>)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMinimumHeight(), 1);
    ASSERT_EQ(control->GetMaximumHeight(), 2);

    xaml = R"(
        <Control>
            <Control.MinimumHeight>3</Control.MinimumHeight>
            <Control.MaximumHeight>4</Control.MaximumHeight>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMinimumHeight(), 3);
    ASSERT_EQ(control->GetMaximumHeight(), 4);
}


TEST(ControlParser, ParseMargin) {

    ASSERT_TRUE(TestFrameProperty<zaf::Control>("Margin", [](zaf::Control& control) {
        return control.GetMargin();
    }));
}


TEST(ControlParser, ParseBorder) {

    ASSERT_TRUE(TestFrameProperty<zaf::Control>("Border", [](zaf::Control& control) {
        return control.GetBorder();
    }));
}


TEST(ControlParser, ParsePadding) {

    ASSERT_TRUE(TestFrameProperty<zaf::Control>("Padding", [](zaf::Control& control) {
        return control.GetPadding();
    }));
}


TEST(ControlParser, ParseIsVisible) {

    ASSERT_TRUE(TestBooleanProperty<zaf::Control>("IsVisible", [](zaf::Control& control) {
        return control.IsVisible();
    }));
}


TEST(ControlParser, ParseIsEnabled) {

    ASSERT_TRUE(TestBooleanProperty<zaf::Control>("IsEnabled", [](zaf::Control& control) {
        return control.IsEnabled();
    }));
}


TEST(ControlParser, ParseCanFocused) {

    ASSERT_TRUE(TestBooleanProperty<zaf::Control>("CanFocused", [](zaf::Control& control) {
        return control.CanFocused();
    }));
}


TEST(ControlParser, ParseCanTabStop) {

    ASSERT_TRUE(TestBooleanProperty<zaf::Control>("CanFocused", [](zaf::Control& control) {
        return control.CanFocused();
    }));
}


TEST(ControlParser, ParseTabIndex) {

    auto xaml = R"(<Control TabIndex="2" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetTabIndex(), 2);

    xaml = R"(<Control><Control.TabIndex>3</Control.TabIndex></Control>)";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetTabIndex(), 3);
}


TEST(ControlParser, ParseColors) {

    auto xaml = R"(
        <Control 
            BackgroundColor="#112233"
            BorderColor="#445566"
        />
    )";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetBackgroundColor(), zaf::Color::FromRGB(0x112233));
    ASSERT_EQ(control->GetBorderColor(), zaf::Color::FromRGB(0x445566));

    xaml = R"(
        <Control>
            <Control.BackgroundColor R="0.1" G="0.2" B="0.3" />
            <Control.BorderColor R="0.4" G="0.5" B="0.6" />
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetBackgroundColor(), zaf::Color(0.1f, 0.2f, 0.3f));
    ASSERT_EQ(control->GetBorderColor(), zaf::Color(0.4f, 0.5f, 0.6f));
}


TEST(ControlParser, ParseChildren) {

    auto xaml = R"(
        <Control>
            <Control Name="child1"></Control>
            <Control Name="child2" />
        </Control>
    )";
    auto control = CreateControlFromXaml(xaml);

    const auto& children = control->GetChildren();
    ASSERT_EQ(children.size(), 2);
    ASSERT_EQ(children[0]->GetName(), L"child1");
    ASSERT_EQ(children[1]->GetName(), L"child2");
}