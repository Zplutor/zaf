#include <gtest/gtest.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/graphic/text/text_format_properties.h>
#include <zaf/graphic/text/text_layout_properties.h>

TEST(TextLayoutTest, GetLineMetrics) {

    auto create_text_layout = [](const std::wstring& text) {
        auto text_format = zaf::GraphicFactory::Instance().CreateTextFormat({});
        zaf::TextLayoutProperties text_layout_properties;
        text_layout_properties.text = text;
        text_layout_properties.text_format = text_format;
        text_layout_properties.width = 10000;
        text_layout_properties.height = 10000;
        return zaf::GraphicFactory::Instance().CreateTextLayout(text_layout_properties);
    };

    //Empty text
    {
        auto text_layout = create_text_layout(L"");
        auto line_metrics = text_layout.GetLineMetrics();
        ASSERT_EQ(line_metrics.size(), 1);
        ASSERT_EQ(line_metrics[0].Length(), 0);
    }

    //Multilines
    {
        auto text_layout = create_text_layout(L"1\r\n22\r\n333");
        auto line_metrics = text_layout.GetLineMetrics();
        ASSERT_EQ(line_metrics.size(), 3);
        ASSERT_EQ(line_metrics[0].Length(), 3);
        ASSERT_EQ(line_metrics[1].Length(), 4);
        ASSERT_EQ(line_metrics[2].Length(), 3);
    }
}