#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/creation.h>
#include <zaf/parsing/parsers/control_parser.h>
#include <zaf/parsing/xaml_reader.h>

namespace {

std::shared_ptr<zaf::Control> CreateControlFromXaml(const std::string& xaml) {

    auto node = zaf::XamlReader::CreateFromString(xaml)->Read();

    auto control = zaf::Create<zaf::Control>();
    zaf::ControlParser parser;
    parser.ParseFromNode(node, *control);
    return control;
}

}


TEST(ControlParser, ParseName) {

    auto xaml = R"(<Control name="xyz" />)";
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

    auto xaml = R"(<Control x="1" y="2" />)";
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

    auto xaml = R"(<Control position="1,2" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetPosition(), zaf::Point(1, 2));

    xaml = R"(
        <Control>
            <Control.Position x="3" y="4" />
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetPosition(), zaf::Point(3, 4));
}


TEST(ControlParser, ParseWidthAndHeight) {

    auto xaml = R"(<Control width="1" height="2" />)";
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

    auto xaml = R"(<Control size="1,2" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetSize(), zaf::Size(1, 2));

    xaml = R"(
        <Control>
            <Control.Size width="3" height="4" />
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetSize(), zaf::Size(3, 4));
}


TEST(ControlParser, ParseRect) {

    auto xaml = R"(
        <Control>
            <Control.Rect position="1,2" size="3,4" />
        </Control>
    )";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetPosition(), zaf::Point(1, 2));
    ASSERT_EQ(control->GetSize(), zaf::Size(3, 4));
}


TEST(ControlParser, ParseMinimumWidthAndMaximumWidth) {

    auto xaml = R"(<Control minimumWidth="1" maximumWidth="2"></Control>)";
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

    auto xaml = R"(<Control minimumHeight="1" maximumHeight="2"></Control>)";
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


TEST(ControlParser, ParseBorder) {

    auto xaml = R"(<Control border="1,2,3,4"></Control>)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetBorder(), zaf::Frame(1, 2, 3, 4));

    xaml = R"(
        <Control>
            <Control.Border left="5" top="6" right="7" bottom="8"></Control.Border>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetBorder(), zaf::Frame(5, 6, 7, 8));
}


TEST(ControlParser, ParsePadding) {

    auto xaml = R"(<Control padding="1,2,3,4"></Control>)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetPadding(), zaf::Frame(1, 2, 3, 4));

    xaml = R"(
        <Control>
            <Control.Padding left="5" top="6" right="7" bottom="8"></Control.Padding>
        </Control>
    )";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetPadding(), zaf::Frame(5, 6, 7, 8));
}


TEST(ControlParser, ParseIsVisible) {

    auto xaml = R"(<Control isVisible="false" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->IsVisible(), false);

    xaml = R"(<Control><Control.IsVisible>false</Control.IsVisible></Control>)";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->IsVisible(), false);
}


TEST(ControlParser, ParseIsEnabled) {

    auto xaml = R"(<Control isEnabled="false" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->IsEnabled(), false);

    xaml = R"(<Control><Control.IsEnabled>false</Control.IsEnabled></Control>)";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->IsEnabled(), false);
}


TEST(ControlParser, ParseCanFocused) {

    auto xaml = R"(<Control canFocused="false" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->CanFocused(), false);

    xaml = R"(<Control><Control.CanFocused>false</Control.CanFocused></Control>)";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->CanFocused(), false);
}


TEST(ControlParser, ParseCanTabStop) {

    auto xaml = R"(<Control canTabStop="false" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->CanTabStop(), false);

    xaml = R"(<Control><Control.CanTabStop>false</Control.CanTabStop></Control>)";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->CanTabStop(), false);
}


TEST(ControlParser, ParseTabIndex) {

    auto xaml = R"(<Control tabIndex="2" />)";
    auto control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetTabIndex(), 2);

    xaml = R"(<Control><Control.TabIndex>3</Control.TabIndex></Control>)";
    control = CreateControlFromXaml(xaml);
    ASSERT_EQ(control->GetTabIndex(), 3);
}


TEST(ControlParser, ParseChildren) {

    auto xaml = R"(
        <Control>
            <Control name="child1"></Control>
            <Control name="child2" />
        </Control>
    )";
    auto control = CreateControlFromXaml(xaml);

    const auto& children = control->GetChildren();
    ASSERT_EQ(children.size(), 2);
    ASSERT_EQ(children[0]->GetName(), L"child1");
    ASSERT_EQ(children[1]->GetName(), L"child2");
}