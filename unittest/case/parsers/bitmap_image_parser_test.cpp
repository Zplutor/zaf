#include <gtest/gtest.h>
#include <zaf/creation.h>
#include <zaf/parsing/helpers.h>
#include <zaf/graphic/image/bitmap_image.h>
#include <zaf/parsing/parsers/bitmap_image_parser.h>

using namespace zaf;

TEST(BitmapImageParser, ParseFromAttribute) {

    auto image = Create<BitmapImage>();

    std::wstring url{ L"file:///C:/image.png" };

    BitmapImageParser parser;
    parser.ParseFromAttribute(url, *image);
    ASSERT_EQ(image->GetUrl(), url);
}


TEST(BitmapImageParser, ParseFromNode) {

    auto image = CreateObjectFromXaml<BitmapImage>(
        R"(<BitmapImage Url="file:///C:/image1.png"></BitmapImage>)"
    );
    ASSERT_EQ(image->GetUrl(), L"file:///C:/image1.png");

    image = CreateObjectFromXaml<BitmapImage>("<BitmapImage>file:///C:/image2.png</BitmapImage>");
    ASSERT_EQ(image->GetUrl(), L"file:///C:/image2.png");

    image = CreateObjectFromXaml<BitmapImage>(R"(
        <BitmapImage>
            <BitmapImage.Url>file:///C:/image3.png</BitmapImage.Url>
        </BitmapImage>
    )");
    ASSERT_EQ(image->GetUrl(), L"file:///C:/image3.png");
}