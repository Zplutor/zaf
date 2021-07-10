#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/graphic/image/bitmap_image.h>
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
    ASSERT_EQ(control->Position(), zaf::Point(1, 2));

    xaml = R"(
        <Control>
            <Control.X>3</Control.X>
            <Control.Y>4</Control.Y>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->Position(), zaf::Point(3, 4));
}


TEST(ControlParserTest, ParsePosition) {

    auto xaml = R"(<Control Position="1,2" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->Position(), zaf::Point(1, 2));

    xaml = R"(
        <Control>
            <Control.Position X="3" Y="4" />
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->Position(), zaf::Point(3, 4));
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
    ASSERT_EQ(control->Position(), zaf::Point(1, 2));
    ASSERT_EQ(control->GetSize(), zaf::Size(3, 4));
}


TEST(ControlParserTest, ParseMinWidthAndMaxWidth) {

    auto xaml = R"(<Control MinWidth="1" MaxWidth="2"></Control>)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMinWidth(), 1);
    ASSERT_EQ(control->GetMaxWidth(), 2);

    xaml = R"(
        <Control>
            <Control.MinWidth>3</Control.MinWidth>
            <Control.MaxWidth>4</Control.MaxWidth>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMinWidth(), 3);
    ASSERT_EQ(control->GetMaxWidth(), 4);
}


TEST(ControlParserTest, ParseMinHeightAndMaxHeight) {

    auto xaml = R"(<Control MinHeight="1" MaxHeight="2"></Control>)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMinHeight(), 1);
    ASSERT_EQ(control->GetMaxHeight(), 2);

    xaml = R"(
        <Control>
            <Control.MinHeight>3</Control.MinHeight>
            <Control.MaxHeight>4</Control.MaxHeight>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMinHeight(), 3);
    ASSERT_EQ(control->GetMaxHeight(), 4);
}


TEST(ControlParserTest, ParseFixedWidth) {

    auto xaml = R"(<Control FixedWidth="1" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMaxWidth(), 1);
    ASSERT_EQ(control->GetMinWidth(), 1);

    xaml = R"(
        <Control>
            <Control.FixedWidth>2</Control.FixedWidth>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMaxWidth(), 2);
    ASSERT_EQ(control->GetMinWidth(), 2);
}


TEST(ControlParserTest, ParseFixedHeight) {

    auto xaml = R"(<Control FixedHeight="3" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMaxHeight(), 3);
    ASSERT_EQ(control->GetMinHeight(), 3);

    xaml = R"(
        <Control>
            <Control.FixedHeight>4</Control.FixedHeight>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMaxHeight(), 4);
    ASSERT_EQ(control->GetMinHeight(), 4);
}


TEST(ControlParserTest, ParseFixedSize) {

    auto xaml = R"(<Control FixedSize="5,6" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMaxWidth(), 5);
    ASSERT_EQ(control->GetMinWidth(), 5);
    ASSERT_EQ(control->GetMaxHeight(), 6);
    ASSERT_EQ(control->GetMinHeight(), 6);

    xaml = R"(
        <Control>
            <Control.FixedSize Width="7" Height="8"></Control.FixedSize>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetMaxWidth(), 7);
    ASSERT_EQ(control->GetMinWidth(), 7);
    ASSERT_EQ(control->GetMaxHeight(), 8);
    ASSERT_EQ(control->GetMinHeight(), 8);
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


TEST(ControlParserTest, ParseBackgroundImage) {

    auto xaml = R"(<Control BackgroundImage="file:///C:/image.png" />)";
    auto control = CreateControlFromXaml(xaml);

    auto image = std::dynamic_pointer_cast<zaf::BitmapImage>(control->GetBackgroundImage());
    ASSERT_NE(image, nullptr);
    ASSERT_EQ(image->GetUri(), L"file:///C:/image.png");

    xaml = R"(
        <Control>
            <Control.BackgroundImage Uri="file:///C:/image.jpg" />
        </Control>
    )";
    control = CreateControlFromXaml(xaml);

    image = std::dynamic_pointer_cast<zaf::BitmapImage>(control->GetBackgroundImage());
    ASSERT_NE(image, nullptr);
    ASSERT_EQ(image->GetUri(), L"file:///C:/image.jpg");
}


TEST(ControlParserTest, ParseBackgroundImageLayout) {

    bool result = TestEnumProperty<zaf::Control, zaf::ImageLayout>(
        "BackgroundImageLayout",
        {
            { zaf::ImageLayout::None, "None" },
            { zaf::ImageLayout::Center, "Center" },
            { zaf::ImageLayout::Stretch, "Stretch" },
            { zaf::ImageLayout::Tile, "Tile" },
            { zaf::ImageLayout::Zoom, "Zoom" },
        },
        [](zaf::Control& control) {
            return control.GetBackgroundImageLayout();
        }
    );
    ASSERT_TRUE(result);
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


TEST(ControlParserTest, ParseAutoSize) {

    //AutoWidth
    auto xaml = R"(<Control AutoWidth="true" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_TRUE(control->AutoWidth());
    ASSERT_FALSE(control->AutoHeight());

    xaml = R"(        
        <Control>
            <Control.AutoWidth>true</Control.AutoWidth>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_TRUE(control->AutoWidth());
    ASSERT_FALSE(control->AutoHeight());

    //AutoHeight
    xaml = R"(<Control AutoHeight="true" />)";
    control = CreateControlFromXaml(xaml);
    ASSERT_FALSE(control->AutoWidth());
    ASSERT_TRUE(control->AutoHeight());

    xaml = R"(        
        <Control>
            <Control.AutoHeight>true</Control.AutoHeight>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_FALSE(control->AutoWidth());
    ASSERT_TRUE(control->AutoHeight());

    //AutoSize
    xaml = R"(<Control AutoSize="true" />)";
    control = CreateControlFromXaml(xaml);
    ASSERT_TRUE(control->AutoWidth());
    ASSERT_TRUE(control->AutoHeight());

    xaml = R"(        
        <Control>
            <Control.AutoSize>true</Control.AutoSize>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_TRUE(control->AutoWidth());
    ASSERT_TRUE(control->AutoHeight());
}