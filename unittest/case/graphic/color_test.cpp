#include <gtest/gtest.h>
#include <zaf/graphic/color.h>
#include <zaf/xml/xml_error.h>
#include <zaf/xml/xml_reader.h>
#include <zaf/xml/xml_writer.h>

using namespace zaf;

TEST(ColorTest, Hash) {

    zaf::Color color1{ 0.1f, 0.2f, 0.3f, 0.4f };
    zaf::Color color2{ 0.1f, 0.2f, 0.3f, 0.4f };
    ASSERT_EQ(color1.Hash(), color2.Hash());
}


TEST(ColorTest, IsEqual) {

    zaf::Color color1{ 0.1f, 0.2f, 0.3f, 0.4f };
    zaf::Color color2{ 0.1f, 0.2f, 0.3f, 0.4f };
    ASSERT_TRUE(color1.IsEqual(color2));

    zaf::Color color3{ 0.4f, 0.2f, 0.3f, 0.4f };
    ASSERT_FALSE(color1.IsEqual(color3));
}


TEST(ColorTest, ToString) {

    zaf::Color color{ 0.1f, 0.2f, 0.3f, 0.4f };
    ASSERT_EQ(color.ToString(), L"0.1,0.2,0.3,0.4");
}


TEST(ColorTest, WriteToXML) {

    auto stream = Stream::FromMemory(0);
    XMLWriter writer{ stream };

    Color color{ 0.1f, 0.2f, 0.3f, 0.4f };
    ASSERT_NO_THROW(color.WriteToXML(writer));
    writer.Flush();

    std::string_view expected = R"(<Color R="0.100000" G="0.200000" B="0.300000" A="0.400000" />)";
    std::string_view actual{
        reinterpret_cast<const char*>(stream.GetUnderlyingBuffer()),
        stream.GetSize()
    };
    ASSERT_EQ(expected, actual);
}


TEST(ColorTest, ReadFromXML) {

    constexpr auto deserialize = [](std::string_view xml) {
        XMLReader reader{ Stream::FromMemoryNoCopy(xml.data(), xml.size()) };
        Color color;
        color.ReadFromXML(reader);
        return color;
    };

    {
        auto color = deserialize(R"(<Color R="0.5" G="0.3" B="0.6" A="0.7" />)");
        ASSERT_EQ(color.a, 0.7f);
        ASSERT_EQ(color.r, 0.5f);
        ASSERT_EQ(color.g, 0.3f);
        ASSERT_EQ(color.b, 0.6f);
    }

    ASSERT_THROW(deserialize(R"(<Color G="0.3" B="0.6" A="0.7" />)"), XMLError);
    ASSERT_THROW(deserialize(R"(<Color R="0.5" B="0.6" A="0.7" />)"), XMLError);
    ASSERT_THROW(deserialize(R"(<Color R="0.5" G="0.3" A="0.7" />)"), XMLError);
    ASSERT_THROW(deserialize(R"(<Color R="0.5" G="0.3" B="0.6" />)"), XMLError);
}