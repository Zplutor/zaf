#include <gtest/gtest.h>
#include <zaf/graphic/font/font.h>
#include <zaf/object/parsing/helpers.h>

TEST(FontPraserTest, ParseFamilyName) {

    auto xaml = LR"(<Font FamilyName="aaaabb" />)";
    auto font = zaf::CreateObjectFromXaml<zaf::Font>(xaml);
    ASSERT_EQ(font->family_name, L"aaaabb");
}


TEST(FontParserTest, ParseSize) {

    auto xaml = LR"(<Font Size="112" />)";
    auto font = zaf::CreateObjectFromXaml<zaf::Font>(xaml);
    ASSERT_EQ(font->size, 112.f);
}


TEST(FontParserTest, ParseWeight) {

    std::pair<const wchar_t*, int> map[] = {
        { L"589", 589 },
        { L"Black", zaf::FontWeight::Black },
        { L"Bold", zaf::FontWeight::Bold },
        { L"ExtraBlack", zaf::FontWeight::ExtraBlack },
        { L"ExtraBold", zaf::FontWeight::ExtraBold },
        { L"ExtraLight", zaf::FontWeight::ExtraLight },
        { L"Light", zaf::FontWeight::Light },
        { L"Maximum", zaf::FontWeight::Maximum },
        { L"Medium", zaf::FontWeight::Medium },
        { L"Minimum", zaf::FontWeight::Minimum },
        { L"Regular", zaf::FontWeight::Regular },
        { L"SemiBold", zaf::FontWeight::SemiBold },
        { L"SemiLight", zaf::FontWeight::SemiLight },
        { L"Thin", zaf::FontWeight::Thin },
    };

    for (const auto& each_pair : map) {
        
        std::wstring xaml(L"<Font Weight=\"");
        xaml.append(each_pair.first);
        xaml.append(L"\" />");
        
        auto font = zaf::CreateObjectFromXaml<zaf::Font>(xaml);
        ASSERT_EQ(font->weight, each_pair.second);
    }
}


TEST(FontParserTest, ParseHasUnderline) {

    auto xaml = LR"(<Font HasUnderline="true" />)";
    auto font = zaf::CreateObjectFromXaml<zaf::Font>(xaml);
    ASSERT_TRUE(font->has_underline);
}