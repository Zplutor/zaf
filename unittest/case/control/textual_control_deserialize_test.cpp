#include <gtest/gtest.h>
#include <zaf/base/string/encoding_conversion.h>
#include <zaf/control/textual_control.h>
#include <zaf/graphic/font/font.h>
#include <zaf/serialization/deserializing.h>


TEST(TextualControl, DeserializeBaseProperty) {

    auto json = "{ \"Name\": \"234\" }";
    auto control = zaf::DeserializeObjectFromJson<zaf::TextualControl>(json);
    ASSERT_EQ(control->GetName(), L"234");
}


TEST(TextualControl, DeserializeText) {

    auto json = "{ \"Text\": \"blbllaal\" }";
    auto control = zaf::DeserializeObjectFromJson<zaf::TextualControl>(json);
    ASSERT_EQ(control->GetText(), L"blbllaal");
}


TEST(TextualControl, DeserializeTextAlignment) {

    auto values = { 
        zaf::TextAlignment::Leading, 
        zaf::TextAlignment::Center, 
        zaf::TextAlignment::Tailing, 
        zaf::TextAlignment::Justified
    };

    for (auto each : values) {

        std::string json("{\"TextAlignment\": \"");
        json.append(zaf::ToUtf8String(zaf::ConvertTextAlignmentToString(each)));
        json.append("\"}");

        auto control = zaf::DeserializeObjectFromJson<zaf::TextualControl>(json);
        ASSERT_EQ(control->GetTextAlignment(), each);
    }

    //Unknown text
    auto json = "{\"TextAlignment\": \"jweiorwoi\"}";
    auto control = zaf::DeserializeObjectFromJson<zaf::TextualControl>(json);
    ASSERT_EQ(control->GetTextAlignment(), zaf::TextAlignment::Leading);
}


TEST(TextualControl, DeserailizeParagraphAlignment) {

    auto values = {
        zaf::ParagraphAlignment::Near,
        zaf::ParagraphAlignment::Center,
        zaf::ParagraphAlignment::Far,
    };

    for (auto each_value : values) {

        std::string json("{\"ParagraphAlignment\": \"");
        json.append(zaf::ToUtf8String(zaf::ConvertParagraphAlignmentToString(each_value)));
        json.append("\"}");

        auto control = zaf::DeserializeObjectFromJson<zaf::TextualControl>(json);
        ASSERT_EQ(control->GetParagraphAlignment(), each_value);
    }

    //Unknown text
    auto json = "{\"ParagraphAlignment\": \"alsfdjalsfjl\"}";
    auto control = zaf::DeserializeObjectFromJson<zaf::TextualControl>(json);
    ASSERT_EQ(control->GetParagraphAlignment(), zaf::ParagraphAlignment::Near);
}


TEST(TextualControl, DeserializeWordWrapping) {

    auto values = {
        zaf::WordWrapping::NoWrap,
        zaf::WordWrapping::Character,
        zaf::WordWrapping::EmergencyBreak,
        zaf::WordWrapping::WholeWord,
        zaf::WordWrapping::Wrap,
    };

    for (auto each_value : values) {

        std::string json("{\"WordWrapping\": \"");
        json.append(zaf::ToUtf8String(zaf::ConvertWordWrappingToString(each_value)));
        json.append("\"}");

        auto control = zaf::DeserializeObjectFromJson<zaf::TextualControl>(json);
        ASSERT_EQ(control->GetWordWrapping(), each_value);
    }

    //Unknown text
    auto json = "{\"WordWrapping\": \"erj2ijk\"}";
    auto control = zaf::DeserializeObjectFromJson<zaf::TextualControl>(json);
    ASSERT_EQ(control->GetWordWrapping(), zaf::WordWrapping::NoWrap);
}


TEST(TextualControl, DeserializeDefaultTextColor) {

    auto json = "{\"DefaultTextColor\":{\"A\":0.8}}";
    auto control = zaf::DeserializeObjectFromJson<zaf::TextualControl>(json);
    ASSERT_EQ(control->GetDefaultTextColor(), zaf::Color(0, 0, 0, 0.8f));
}


TEST(TextualControl, DeserializeDefaultTextColorPicker) {

    auto json = "{\"DefaultTextColorPicker\":{ \"Color\": { \"R\": 0.55, \"B\": 0.67 } }}";
    auto control = zaf::DeserializeObjectFromJson<zaf::TextualControl>(json);
    ASSERT_EQ(control->GetDefaultTextColor(), zaf::Color(0.55f, 0, 0.67f));
}


TEST(TextualControl, DeserializeDefaultFont) {

    auto control = zaf::DeserializeObjectFromJson<zaf::TextualControl>("{\"DefaultFont\":{\"FamilyName\":\"Song\"}}");
    ASSERT_EQ(control->GetDefaultFont().family_name, L"Song");
}


TEST(TextualControl, DeserializeTextTrimming) {

    auto control = zaf::DeserializeObjectFromJson<zaf::TextualControl>("{\"TextTrimming\":{\"Granularity\": \"Word\"}}");
    ASSERT_EQ(control->GetTextTrimming().granularity, zaf::TextTrimming::Granularity::Word);
}