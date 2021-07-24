#include <gtest/gtest.h>
#include <zaf/creation.h>
#include <zaf/object/parsing/helpers.h>
#include <zaf/object/object_type.h>
#include <zaf/graphic/image/uri_image.h>

using namespace zaf;

TEST(URIImageParser, ParseFromAttribute) {

    auto image = Create<URIImage>();

    std::wstring uri{ L"file:///C:/image.png" };

    auto parser = URIImage::Type->GetParser();
    parser->ParseFromAttribute(uri, *image);
    ASSERT_EQ(image->GetURI(), uri);
}


TEST(URIImageParser, ParseFromNode) {

    auto image = CreateObjectFromXaml<URIImage>(
        R"(<URIImage Uri="file:///C:/image1.png"></URIImage>)"
    );
    ASSERT_EQ(image->GetURI(), L"file:///C:/image1.png");

    image = CreateObjectFromXaml<URIImage>("<URIImage>file:///C:/image2.png</URIImage>");
    ASSERT_EQ(image->GetURI(), L"file:///C:/image2.png");

    image = CreateObjectFromXaml<URIImage>(R"(
        <URIImage>
            <URIImage.Uri>file:///C:/image3.png</URIImage.Uri>
        </URIImage>
    )");
    ASSERT_EQ(image->GetURI(), L"file:///C:/image3.png");
}