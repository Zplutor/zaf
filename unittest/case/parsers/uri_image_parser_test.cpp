#include <gtest/gtest.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/creation.h>
#include <zaf/object/parsing/helpers.h>
#include <zaf/object/parsing/xaml_reader.h>
#include <zaf/object/object_type.h>
#include <zaf/graphic/image/uri_image.h>
#include "utility/assert.h"

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

    auto parser = URIImage::Type->GetParser();
    zaf::Object object;

    ASSERT_THROW_ERRC(
        parser->ParseFromAttribute(L"file:///C:/image.png", object), 
        zaf::BasicErrc::InvalidCast);

    auto xaml_node = XamlReader::FromString(L"<URIImage>file:///C:/image2.png</URIImage>")->Read();
    ASSERT_THROW_ERRC(parser->ParseFromNode(*xaml_node, object), zaf::BasicErrc::InvalidCast);
}