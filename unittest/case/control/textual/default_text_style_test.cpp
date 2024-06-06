#include <gtest/gtest.h>
#include <zaf/control/textual/default_text_style.h>
#include <zaf/xml/xml_error.h>
#include <zaf/xml/xml_reader.h>
#include <zaf/xml/xml_writer.h>

using namespace zaf;
using namespace zaf::textual;

TEST(DefaultTextStyleTest, WriteToXML) {

    DefaultTextStyle style;
    style.SetFont(Font{ L"Default", 30.f, 400 });
    style.SetTextColor(Color{ 0, 0, 0 });
    style.SetTextBackColor(Color{ 0.3f, 0.3f, 0.3f, 0.4f });

    auto stream = Stream::FromMemory(0);
    XMLWriter writer{ stream };

    style.WriteToXML(writer);
    writer.Flush();

    std::string_view expected =
        R"(<DefaultTextStyle>)"
        R"(<Font>)"
        R"(<Font FamilyName="Default" Size="30.000000" Weight="400" )"
        R"(Style="Normal" HasUnderline="false" />)"
        R"(</Font>)"
        R"(<TextColor>)"
        R"(<Color R="0.000000" G="0.000000" B="0.000000" A="1.000000" />)"
        R"(</TextColor>)"
        R"(<TextBackColor>)"
        R"(<Color R="0.300000" G="0.300000" B="0.300000" A="0.400000" />)"
        R"(</TextBackColor>)"
        R"(</DefaultTextStyle>)";

    std::string_view actual{
        reinterpret_cast<const char*>(stream.GetUnderlyingBuffer()),
        stream.GetSize()
    };
    ASSERT_EQ(expected, actual);
}


TEST(DefaultTextStyleTest, ReadFromXML) {

    constexpr auto deserialize = [](std::string_view xml) {
        XMLReader reader{ Stream::CreateOnMemory(xml.data(), xml.size()) };
        DefaultTextStyle style;
        style.ReadFromXML(reader);
        return style;
    };

    {
        auto style = deserialize(
            R"(<DefaultTextStyle>)"
            R"(<Font>)"
            R"(<Font FamilyName="Default" Size="3.000000" Weight="500" )"
            R"(Style="Normal" HasUnderline="true" />)"
            R"(</Font>)"
            R"(<TextColor>)"
            R"(<Color R="0.000000" G="0.000000" B="0.000000" A="1.000000" />)"
            R"(</TextColor>)"
            R"(<TextBackColor>)"
            R"(<Color R="0.400000" G="0.400000" B="0.400000" A="0.400000" />)"
            R"(</TextBackColor>)"
            R"(</DefaultTextStyle>)");

        ASSERT_EQ(style.Font().family_name, L"Default");
        ASSERT_EQ(style.Font().size, 3.f);
        ASSERT_EQ(style.Font().weight, 500);
        ASSERT_EQ(style.Font().style, FontStyle::Normal);
        ASSERT_EQ(style.Font().has_underline, true);
        ASSERT_EQ(style.TextColor(), Color(0.f, 0.f, 0.f, 1.f));
        ASSERT_EQ(style.TextBackColor(), Color(0.4f, 0.4f, 0.4f, 0.4f));
    }

    ASSERT_THROW(
        deserialize(
            R"(<DefaultTextStyle>)"
            R"(<TextColor>)"
            R"(<Color R="0.000000" G="0.000000" B="0.000000" A="1.000000" />)"
            R"(</TextColor>)"
            R"(<TextBackColor>)"
            R"(<Color R="0.400000" G="0.400000" B="0.400000" A="0.400000" />)"
            R"(</TextBackColor>)"
            R"(</DefaultTextStyle>)"),
        XMLError);

    ASSERT_THROW(
        deserialize(
            R"(<DefaultTextStyle>)"
            R"(<Font>)"
            R"(<Font FamilyName="Default" Size="3.000000" Weight="500" )"
            R"(Style="Normal" HasUnderline="true" />)"
            R"(</Font>)"
            R"(<TextBackColor>)"
            R"(<Color R="0.400000" G="0.400000" B="0.400000" A="0.400000" />)"
            R"(</TextBackColor>)"
            R"(</DefaultTextStyle>)"),
        XMLError);

    ASSERT_THROW(
        deserialize(
            R"(<DefaultTextStyle>)"
            R"(<Font>)"
            R"(<Font FamilyName="Default" Size="3.000000" Weight="500" )"
            R"(Style="Normal" HasUnderline="true" />)"
            R"(</Font>)"
            R"(<TextColor>)"
            R"(<Color R="0.000000" G="0.000000" B="0.000000" A="1.000000" />)"
            R"(</TextColor>)"
            R"(</DefaultTextStyle>)"),
        XMLError);
}