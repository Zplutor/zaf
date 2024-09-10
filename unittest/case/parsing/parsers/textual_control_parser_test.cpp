#include <gtest/gtest.h>
#include <zaf/control/textual_control.h>
#include <zaf/graphic/font/font.h>
#include "utility.h"

std::shared_ptr<zaf::TextualControl> CreateTextualControlFromXaml(const std::string& xaml) {
    return zaf::CreateObjectFromXaml<zaf::TextualControl>(xaml);
}


TEST(TextualControlParser, ParseText) {

    auto xaml = R"(<TextualControl Text="ttttt"></TextualControl>)";
    auto control = CreateTextualControlFromXaml(xaml);
    ASSERT_EQ(control->Text(), L"ttttt");

    xaml = R"(
        <TextualControl>
            <TextualControl.Text>eeeeeee</TextualControl.Text>
        </TextualControl>
    )";
    control = CreateTextualControlFromXaml(xaml);
    ASSERT_EQ(control->Text(), L"eeeeeee");
}


TEST(TextualControlParser, ParseTextColor) {

    auto xaml = R"(<TextualControl TextColor="#aabbcc" />)";
    auto control = CreateTextualControlFromXaml(xaml);
    ASSERT_EQ(control->TextColor(), zaf::Color::FromRGB(0xaabbcc));

    xaml = R"(
        <TextualControl>
            <TextualControl.TextColor R="0.3" G="0.4" B="0.5" />
        </TextualControl>
    )";
    control = CreateTextualControlFromXaml(xaml);
    ASSERT_EQ(control->TextColor(), zaf::Color(0.3f, 0.4f, 0.5f));
}


TEST(TextualControlParser, ParseTextAlignment) {

    auto test = [](const std::string& value_string, zaf::dwrite::TextAlignment expected_value) {
    
        std::string xaml = R"(<TextualControl TextAlignment=")" + value_string + R"("/>)";
        auto control = CreateTextualControlFromXaml(xaml);
        ASSERT_EQ(control->TextAlignment(), expected_value);

        xaml = 
            "<TextualControl><TextualControl.TextAlignment>" + 
            value_string + 
            "</TextualControl.TextAlignment></TextualControl>";
        control = CreateTextualControlFromXaml(xaml);
        ASSERT_EQ(control->TextAlignment(), expected_value);
    };

    test("Leading", zaf::dwrite::TextAlignment::Leading);
    test("Tailing", zaf::dwrite::TextAlignment::Tailing);
    test("Center", zaf::dwrite::TextAlignment::Center);
    test("Justified", zaf::dwrite::TextAlignment::Justified);
}


TEST(TextualControlParser, ParseParagraphAlignment) {

    auto test = [](
        const std::string& value_string, 
        zaf::dwrite::ParagraphAlignment expected_value) {
    
        std::string xaml = R"(<TextualControl ParagraphAlignment=")" + value_string + R"("/>)";
        auto control = CreateTextualControlFromXaml(xaml);
        ASSERT_EQ(control->ParagraphAlignment(), expected_value);

        xaml =
            "<TextualControl><TextualControl.ParagraphAlignment>" +
            value_string +
            "</TextualControl.ParagraphAlignment></TextualControl>";
        control = CreateTextualControlFromXaml(xaml);
        ASSERT_EQ(control->ParagraphAlignment(), expected_value);
    };

    test("Near", zaf::dwrite::ParagraphAlignment::Near);
    test("Far", zaf::dwrite::ParagraphAlignment::Far);
    test("Center", zaf::dwrite::ParagraphAlignment::Center);
}


TEST(TextualControlParser, ParseWordWrapping) {

    auto test = [](const std::string& value_string, zaf::dwrite::WordWrapping expected_value) {
    
        std::string xaml = R"(<TextualControl WordWrapping=")" + value_string + R"("/>)";
        auto control = CreateTextualControlFromXaml(xaml);
        ASSERT_EQ(control->WordWrapping(), expected_value);

        xaml =
            "<TextualControl><TextualControl.WordWrapping>" +
            value_string +
            "</TextualControl.WordWrapping></TextualControl>";
        control = CreateTextualControlFromXaml(xaml);
        ASSERT_EQ(control->WordWrapping(), expected_value);
    };

    test("Wrap", zaf::dwrite::WordWrapping::Wrap);
    test("NoWrap", zaf::dwrite::WordWrapping::NoWrap);
    test("EmergencyBreak", zaf::dwrite::WordWrapping::EmergencyBreak);
    test("WholeWord", zaf::dwrite::WordWrapping::WholeWord);
    test("Character", zaf::dwrite::WordWrapping::Character);
}


TEST(TextualControlParser, ParseFont) {

    auto xaml = R"(
        <TextualControl>
            <TextualControl.Font FamilyName="ab" Size="11" Weight="100" />
        </TextualControl>
    )";
    auto control = CreateTextualControlFromXaml(xaml);
    auto font = control->Font();
    ASSERT_EQ(font.family_name, L"ab");
    ASSERT_EQ(font.size, 11);
    ASSERT_EQ(font.weight, 100);
}


TEST(TextualControlParser, ParseFontSize) {

    auto xaml = R"(<TextualControl FontSize="13"/>)";
    auto control = CreateTextualControlFromXaml(xaml);
    ASSERT_EQ(control->FontSize(), 13);
}


TEST(TextualControlParser, ParseFontWeight) {

    auto xaml = R"(<TextualControl FontWeight="443"/>)";
    auto control = CreateTextualControlFromXaml(xaml);
    ASSERT_EQ(control->FontWeight(), 443);

    xaml = R"(<TextualControl FontWeight="Bold"/>)";
    control = CreateTextualControlFromXaml(xaml);
    ASSERT_EQ(control->FontWeight(), zaf::FontWeight::Bold);
}

