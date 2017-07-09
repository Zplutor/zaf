#include <gtest/gtest.h>
#include <zaf/base/string/encoding_conversion.h>
#include <zaf/graphic/text/text_trimming.h>
#include "utility/deserialize_utility.h"

TEST(TextTrimming, DeserializeGranularity) {

    auto granularities = { 
        zaf::TextTrimming::Granularity::None,
        zaf::TextTrimming::Granularity::Character,
        zaf::TextTrimming::Granularity::Word,
    };

    for (auto each : granularities) {

        std::string json("{\"Granularity\":\"");
        json.append(zaf::ToUtf8String(zaf::ConvertTextTrimmingGranularityToString(each)));
        json.append("\"}");

        auto text_trimming = DeserializeObject<zaf::TextTrimming>(json);
        ASSERT_EQ(text_trimming->granularity, each);
    }

    //Unknown text
    auto text_trimming = DeserializeObject<zaf::TextTrimming>("{\"Granularity\": \"safasfs\"}");
    ASSERT_EQ(text_trimming->granularity, zaf::TextTrimming::Granularity::None);
}


TEST(TextTrimming, DeserializeDelimiter) {

    auto text_trimming = DeserializeObject<zaf::TextTrimming>("{\"Delimiter\": \"|\"}");
    ASSERT_EQ(text_trimming->delimiter, L'|');

    //Invalid value
    text_trimming = DeserializeObject<zaf::TextTrimming>("{\"Delimiter\": \"\"}");
    ASSERT_EQ(text_trimming->delimiter, 0);

    text_trimming = DeserializeObject<zaf::TextTrimming>("{\"Delimiter\": \"||\"}");
    ASSERT_EQ(text_trimming->delimiter, 0);
}


TEST(TextTrimming, DeserializeDelimiterCount) {

    auto text_trimming = DeserializeObject<zaf::TextTrimming>("{\"DelimiterCount\": 2}");
    ASSERT_EQ(text_trimming->delimiter_count, 2);
}