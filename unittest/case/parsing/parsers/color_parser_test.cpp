#include <gtest/gtest.h>
#include <zaf/graphic/color.h>
#include <zaf/parsing/parsers/color_parser.h>
#include <zaf/parsing/xaml_node.h>
#include <zaf/parsing/xaml_reader.h>
#include "utility.h"

TEST(ColorParser, ParseFromAttribute) {

    zaf::Color color;
    zaf::ColorParser parser;

    parser.ParseFromAttribute(L"#112233", color);
    ASSERT_EQ(color, zaf::Color::FromRGB(0x112233));

    parser.ParseFromAttribute(L"#44556677", color);
    ASSERT_EQ(color, zaf::Color::FromARGB(0x44556677));

    struct MapItem {
        const wchar_t* name;
        const zaf::Color* color;
    };
    static const MapItem map[] = {
        { L"Black", &zaf::Color::Black() },
        { L"Blue", &zaf::Color::Blue() },
        { L"Cyan", &zaf::Color::Cyan() },
        { L"Gray", &zaf::Color::Gray() },
        { L"Green", &zaf::Color::Green() },
        { L"Lime", &zaf::Color::Lime() },
        { L"Magenta", &zaf::Color::Magenta() },
        { L"Red", &zaf::Color::Red() },
        { L"Transparent", &zaf::Color::Transparent() },
        { L"White", &zaf::Color::White() },
        { L"Yellow", &zaf::Color::Yellow() },
    };
    for (const auto& each_item : map) {
        parser.ParseFromAttribute(each_item.name, color);
        ASSERT_EQ(color, *each_item.color);
    }
}


TEST(ColorParser, ParseFromNode) {

    auto color = zaf::CreateObjectFromXaml<zaf::Color>(
        R"(<Color A="0.1" R="0.2" G="0.3" B="0.4"></Color>)"
    );
    ASSERT_EQ(*color, zaf::Color(0.2f, 0.3f, 0.4f, 0.1f));

    color = zaf::CreateObjectFromXaml<zaf::Color>("<Color>#ddeeff</Color>");
    ASSERT_EQ(*color, zaf::Color::FromRGB(0xddeeff));

    color = zaf::CreateObjectFromXaml<zaf::Color>(R"(
        <Color>
            <Color.A>0.5</Color.A>
            <Color.R>0.6</Color.R>
            <Color.G>0.7</Color.G>
            <Color.B>0.8</Color.B>
        </Color>
    )");
    ASSERT_EQ(*color, zaf::Color(0.6f, 0.7f, 0.8f, 0.5f));

    color = zaf::CreateObjectFromXaml<zaf::Color>("<Color>Cyan</Color>");
    ASSERT_EQ(*color, zaf::Color::Cyan());
}