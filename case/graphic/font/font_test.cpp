#include <gtest/gtest.h>
#include <zaf/graphic/font/font.h>
#include "utility/deserialize_utility.h"

TEST(Font, Deserialize) {

    auto font = DeserializeObject<zaf::Font>("{\"FamilyName\": \"Song\", \"Size\": 11.1}");
    ASSERT_NE(font, nullptr);
    ASSERT_EQ(font->family_name, L"Song");
    ASSERT_EQ(font->size, 11.1f);
}


TEST(Font, DeserializeWeight) {

    std::pair<const char*, int> map[] = {
        { "Black", zaf::FontWeight::Black },
        { "Bold", zaf::FontWeight::Bold },
        { "ExtraBlack", zaf::FontWeight::ExtraBlack },
        { "ExtraBold", zaf::FontWeight::ExtraBold },
        { "ExtraLight", zaf::FontWeight::ExtraLight },
        { "Light", zaf::FontWeight::Light },
        { "Maximum", zaf::FontWeight::Maximum },
        { "Medium", zaf::FontWeight::Medium },
        { "Minimum", zaf::FontWeight::Minimum },
        { "Regular", zaf::FontWeight::Regular },
        { "SemiBold", zaf::FontWeight::SemiBold },
        { "SemiLight", zaf::FontWeight::SemiLight },
        { "Thin", zaf::FontWeight::Thin },
    };

    for (const auto& each_pair : map) {

        std::string json("{\"Weight\":\"");
        json.append(each_pair.first);
        json.append("\"}");
        auto font = DeserializeObject<zaf::Font>(json);
        ASSERT_EQ(font->weight, each_pair.second);
    }

    auto font = DeserializeObject<zaf::Font>("{\"Weight\": 300}");
    ASSERT_EQ(font->weight, 300);

    font = DeserializeObject<zaf::Font>("{\"Weight\": 0}");
    ASSERT_EQ(font->weight, zaf::FontWeight::Regular);

    font = DeserializeObject<zaf::Font>("{\"Weight\": 1000}");
    ASSERT_EQ(font->weight, zaf::FontWeight::Regular);
}