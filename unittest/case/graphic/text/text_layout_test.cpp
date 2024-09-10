#include <gtest/gtest.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/graphic/dwrite/text_format_properties.h>

TEST(TextLayoutTest, GetLineMetrics) {

    auto create_text_layout = [](const std::wstring& text) {
        auto text_format = zaf::GraphicFactory::Instance().CreateTextFormat({});
        return zaf::GraphicFactory::Instance().CreateTextLayout(
            text,
            text_format,
            zaf::Size{ 10000, 10000 });
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