#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/control/layout/linear_layouter.h>
#include "utility.h"

namespace {

std::shared_ptr<zaf::Control> CreateControlFromXaml(const std::string& xaml) {
    return zaf::CreateObjectFromXaml<zaf::Control>(xaml);
}

}


TEST(ControlParserTest, ParseName) {

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


TEST(ControlParserTest, ParseXAndY) {

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


TEST(ControlParserTest, ParsePosition) {

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


TEST(ControlParserTest, ParseWidthAndHeight) {

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


TEST(ControlParserTest, ParseSize) {

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


TEST(ControlParserTest, ParseRect) {

    auto xaml = R"(
        <Control>
            <Control.Rect Position="1,2" Size="3,4" />
        </Control>
    )";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetPosition(), zaf::Point(1, 2));
    ASSERT_EQ(control->GetSize(), zaf::Size(3, 4));
}


TEST(ControlParserTest, ParseMinimumWidthAndMaximumWidth) {

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


TEST(ControlParserTest, ParseMinimumHeightAndMaximumHeight) {

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


TEST(ControlParserTest, ParseFixedWidth) {

    auto xaml = R"(<Control FixedWidth="1" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMaximumWidth(), 1);
    ASSERT_EQ(control->GetMinimumWidth(), 1);

    xaml = R"(
        <Control>
            <Control.FixedWidth>2</Control.FixedWidth>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMaximumWidth(), 2);
    ASSERT_EQ(control->GetMinimumWidth(), 2);
}


TEST(ControlParserTest, ParseFixedHeight) {

    auto xaml = R"(<Control FixedHeight="3" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMaximumHeight(), 3);
    ASSERT_EQ(control->GetMinimumHeight(), 3);

    xaml = R"(
        <Control>
            <Control.FixedHeight>4</Control.FixedHeight>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMaximumHeight(), 4);
    ASSERT_EQ(control->GetMinimumHeight(), 4);
}


TEST(ControlParserTest, ParseFixedSize) {

    auto xaml = R"(<Control FixedSize="5,6" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMaximumWidth(), 5);
    ASSERT_EQ(control->GetMinimumWidth(), 5);
    ASSERT_EQ(control->GetMaximumHeight(), 6);
    ASSERT_EQ(control->GetMinimumHeight(), 6);

    xaml = R"(
        <Control>
            <Control.FixedSize Width="7" Height="8"></Control.FixedSize>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMaximumWidth(), 7);
    ASSERT_EQ(control->GetMinimumWidth(), 7);
    ASSERT_EQ(control->GetMaximumHeight(), 8);
    ASSERT_EQ(control->GetMinimumHeight(), 8);
}


TEST(ControlParserTest, ParseMargin) {

    ASSERT_TRUE(TestFrameProperty<zaf::Control>("Margin", [](zaf::Control& control) {
        return control.GetMargin();
    }));
}


TEST(ControlParserTest, ParseBorder) {

    ASSERT_TRUE(TestFrameProperty<zaf::Control>("Border", [](zaf::Control& control) {
        return control.GetBorder();
    }));
}


TEST(ControlParserTest, ParsePadding) {

    ASSERT_TRUE(TestFrameProperty<zaf::Control>("Padding", [](zaf::Control& control) {
        return control.GetPadding();
    }));
}


TEST(ControlParserTest, ParseIsVisible) {

    ASSERT_TRUE(TestBooleanProperty<zaf::Control>("IsVisible", [](zaf::Control& control) {
        return control.IsVisible();
    }));
}


TEST(ControlParserTest, ParseIsEnabled) {

    ASSERT_TRUE(TestBooleanProperty<zaf::Control>("IsEnabled", [](zaf::Control& control) {
        return control.IsEnabled();
    }));
}


TEST(ControlParserTest, ParseCanFocused) {

    ASSERT_TRUE(TestBooleanProperty<zaf::Control>("CanFocused", [](zaf::Control& control) {
        return control.CanFocused();
    }));
}


TEST(ControlParserTest, ParseCanTabStop) {

    ASSERT_TRUE(TestBooleanProperty<zaf::Control>("CanFocused", [](zaf::Control& control) {
        return control.CanFocused();
    }));
}


TEST(ControlParserTest, ParseTabIndex) {

    auto xaml = R"(<Control TabIndex="2" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetTabIndex(), 2);

    xaml = R"(<Control><Control.TabIndex>3</Control.TabIndex></Control>)";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetTabIndex(), 3);
}


TEST(ControlParserTest, ParseColors) {

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


TEST(ControlParserTest, ParseLayouter) {

    auto xaml = R"(<Control Layouter="LinearLayouter" />)";
    auto control = CreateControlFromXaml(xaml);
    auto layouter = std::dynamic_pointer_cast<zaf::LinearLayouter>(control->GetLayouter());
    ASSERT_NE(layouter, nullptr);

    xaml = R"(
        <Control>
            <Control.Layouter>
                <LinearLayouter ControlAlignment="Center" AxisAlignment="Center" />
            </Control.Layouter>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    layouter = std::dynamic_pointer_cast<zaf::LinearLayouter>(control->GetLayouter());
    ASSERT_NE(layouter, nullptr);
    ASSERT_EQ(layouter->GetControlAlignment(), zaf::ControlAlignment::Center);
    ASSERT_EQ(layouter->GetAxisAlignment(), zaf::AxisAlignment::Center);
}


TEST(ControlParserTest, ParseChildren) {

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