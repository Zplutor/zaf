#include <gtest/gtest.h>
#include <zaf/creation.h>
#include <zaf/object/parsing/helpers.h>
#include <zaf/graphic/image/bitmap_image.h>
#include <zaf/parsing/parsers/bitmap_image_parser.h>

using namespace zaf;

TEST(BitmapImageParser, ParseFromAttribute) {

    auto image = Create<BitmapImage>();

    std::wstring uri{ L"file:///C:/image.png" };

    BitmapImageParser parser;
    parser.ParseFromAttribute(uri, *image);
    ASSERT_EQ(image->GetUri(), uri);
}


TEST(BitmapImageParser, ParseFromNode) {

    auto image = CreateObjectFromXaml<BitmapImage>(
        R"(<BitmapImage Uri="file:///C:/image1.png"></BitmapImage>)"
    );
    ASSERT_EQ(image->GetUri(), L"file:///C:/image1.png");

    image = CreateObjectFromXaml<BitmapImage>("<BitmapImage>file:///C:/image2.png</BitmapImage>");
    ASSERT_EQ(image->GetUri(), L"file:///C:/image2.png");

    image = CreateObjectFromXaml<BitmapImage>(R"(
        <BitmapImage>
            <BitmapImage.Uri>file:///C:/image3.png</BitmapImage.Uri>
        </BitmapImage>
    )");
    ASSERT_EQ(image->GetUri(), L"file:///C:/image3.png");
}