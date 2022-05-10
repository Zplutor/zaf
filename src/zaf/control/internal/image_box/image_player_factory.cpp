#include <zaf/control/internal/image_box/image_player_factory.h>
#include <zaf/application.h>
#include <zaf/graphic/image/wic/imaging_factory.h>
#include <zaf/control/internal/image_box/gif_player.h>
#include <zaf/control/internal/image_box/static_image_player.h>
#include <zaf/control/internal/image_box/uri_image_player.h>

namespace zaf::internal {

std::unique_ptr<ImagePlayer> ImagePlayerFactory::CreateWithImage(
    const std::shared_ptr<Image>& image) {

    return std::make_unique<StaticImagePlayer>(image);
}


std::unique_ptr<ImagePlayer> ImagePlayerFactory::CreateWithDecoder(
    const wic::BitmapDecoder& bitmap_decoder) {

    auto container_format = bitmap_decoder.GetContainerFormat();
    switch (container_format) {
    case wic::ContainerFormat::Gif:
        return std::make_unique<GifPlayer>(bitmap_decoder);

    default: 
        return CreateWithImage(Image::FromBitmapDecoder(bitmap_decoder));
    }
}


std::unique_ptr<ImagePlayer> ImagePlayerFactory::CreateWithFilePath(
    const std::filesystem::path& path) {

    auto decoder = GetImagingFactory().CreateDecoderFromFile(path);
    return CreateWithDecoder(decoder);
}


std::unique_ptr<ImagePlayer> ImagePlayerFactory::CreateWithURI(const std::wstring& uri, float dpi) {

    return std::make_unique<URIImagePlayer>(uri, dpi);
}

}