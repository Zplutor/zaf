#include <gtest/gtest.h>
#include <zaf/base/error/invalid_type_error.h>
#include <zaf/creation.h>
#include <zaf/object/parsing/helpers.h>
#include <zaf/object/parsing/xaml_reader.h>
#include <zaf/object/object_type.h>
#include <zaf/graphic/image/image.h>

using namespace zaf;

TEST(URIImageParser, ParseFromAttribute) {

    auto image = Create<URIImage>();

    std::wstring uri{ L"file:///C:/image.png" };

    auto parser = URIImage::StaticType()->GetParser();
    parser->ParseFromAttribute(uri, *image);
    ASSERT_EQ(image->GetURI(), uri);
}


TEST(URIImageParser, ParseFromNode) {

    auto image = CreateObjectFromXaml<URIImage>(
        R"(<URIImage URI="file:///C:/image1.png"></URIImage>)"
    );
    ASSERT_EQ(image->GetURI(), L"file:///C:/image1.png");

    image = CreateObjectFromXaml<URIImage>("<URIImage>file:///C:/image2.png</URIImage>");
    ASSERT_EQ(image->GetURI(), L"file:///C:/image2.png");

    image = CreateObjectFromXaml<URIImage>(R"(
        <URIImage>
            <URIImage.URI>file:///C:/image3.png</URIImage.URI>
        </URIImage>
    )");
    ASSERT_EQ(image->GetURI(), L"file:///C:/image3.png");
}


TEST(URIImageParser, ParseToInvalidObject) {

    auto parser = URIImage::StaticType()->GetParser();
    zaf::Object object;

    ASSERT_THROW(
        parser->ParseFromAttribute(L"file:///C:/image.png", object), 
        zaf::InvalidTypeError);

    auto xaml_node = XamlReader::FromString(L"<URIImage>file:///C:/image2.png</URIImage>")->Read();
    ASSERT_THROW(parser->ParseFromNode(*xaml_node, object), zaf::InvalidTypeError);
}