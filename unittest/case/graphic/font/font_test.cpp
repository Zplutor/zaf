#include <gtest/gtest.h>
#include <zaf/graphic/font/font.h>

TEST(FontTest, Construction) {

    //Default construction
    {
        zaf::Font font;
        ASSERT_EQ(font.family_name, L"");
        ASSERT_EQ(font.size, 0);
        ASSERT_EQ(font.weight, zaf::FontWeight::Regular);
        ASSERT_EQ(font.has_underline, false);
    }

    //One argument
    {
        zaf::Font font{ L"ddef" };
        ASSERT_EQ(font.family_name, L"ddef");
        ASSERT_EQ(font.size, 0);
        ASSERT_EQ(font.weight, zaf::FontWeight::Regular);
        ASSERT_EQ(font.has_underline, false);
    }

    //Two arguments
    {
        zaf::Font font{ L"tts", 88 };
        ASSERT_EQ(font.family_name, L"tts");
        ASSERT_EQ(font.size, 88);
        ASSERT_EQ(font.weight, zaf::FontWeight::Regular);
        ASSERT_EQ(font.has_underline, false);
    }

    //Three arguments
    {
        zaf::Font font{ L"zae", 11, zaf::FontWeight::ExtraLight };
        ASSERT_EQ(font.family_name, L"zae");
        ASSERT_EQ(font.size, 11);
        ASSERT_EQ(font.weight, zaf::FontWeight::ExtraLight);
        ASSERT_EQ(font.has_underline, false);
    }
}


TEST(FontTest, Comparison) {

    zaf::Font font1;
    font1.family_name = L"consola";
    font1.size = 12;
    font1.weight = zaf::FontWeight::Thin;
    font1.has_underline = false;

    //Equal
    auto font2 = font1;
    ASSERT_TRUE(font1 == font2);

    //Not equal
    font2 = font1;
    font2.family_name = L"Sim";
    ASSERT_FALSE(font1 == font2);
    ASSERT_TRUE(font1 != font2);

    font2 = font1;
    font2.size = 11;
    ASSERT_FALSE(font1 == font2);
    ASSERT_TRUE(font1 != font2);

    font2 = font1;
    font2.weight = zaf::FontWeight::Regular;
    ASSERT_FALSE(font1 == font2);
    ASSERT_TRUE(font1 != font2);

    font2 = font1;
    font2.has_underline = true;
    ASSERT_FALSE(font1 == font2);
    ASSERT_TRUE(font1 != font2);

    //Less
    font2 = font1;
    font2.family_name = L"d";
    ASSERT_TRUE(font1 < font2);

    font2 = font1;
    font2.size = 13;
    ASSERT_TRUE(font1 < font2);

    font2 = font1;
    font2.weight = zaf::FontWeight::Regular;
    ASSERT_TRUE(font1 < font2);

    font2 = font1;
    font2.has_underline = true;
    ASSERT_TRUE(font1 < font2);
}


TEST(FontTest, Hash) {

    zaf::Font font1;
    font1.family_name = L"aa";
    font1.size = 8;
    font1.weight = zaf::FontWeight::ExtraBold;
    font1.has_underline = true;

    auto font2 = font1;
    ASSERT_EQ(font1.Hash(), font2.Hash());
}


TEST(FontTest, IsEqual) {

    zaf::Font font1;
    font1.family_name = L"aa";
    font1.size = 8;
    font1.weight = zaf::FontWeight::ExtraBold;
    font1.has_underline = true;

    auto font2 = font1;
    ASSERT_TRUE(font1.IsEqual(font2));
    ASSERT_TRUE(font2.IsEqual(font1));

    font2.size = 9;
    ASSERT_FALSE(font1.IsEqual(font2));
    ASSERT_FALSE(font2.IsEqual(font1));
}


TEST(FontTest, ToString) {

    zaf::Font font;
    font.family_name = L"consola";
    font.size = 23;
    font.weight = zaf::FontWeight::Thin;
    font.has_underline = true;

    ASSERT_EQ(font.ToString(), L"consola,23,100");
}