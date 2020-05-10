#include <zaf/graphic/image/image.h>
#include <zaf/application.h>
#include <zaf/graphic/image/internal/utility.h>
#include <zaf/graphic/image/internal/wic_bitmap.h>
#include <zaf/graphic/image/wic/imaging_factory.h>

namespace zaf {

std::shared_ptr<Image> Image::FromFile(
    const std::filesystem::path& file_path,
    std::error_code& error_code) {

    auto bitmap_decoder = GetApplication().GetImagingFactory().CreateDecoderFromFile(
        file_path,
        error_code);

    if (!IsSucceeded(error_code)) {
        return {};
    }

    return FromBitmapDecoder(bitmap_decoder);
}


std::shared_ptr<Image> Image::FromBitmap(const wic::BitmapSource& wic_bitmap) {
    return std::make_shared<internal::WicBitmap>(wic_bitmap);
}


std::shared_ptr<Image> Image::FromBitmapDecoder(
    const wic::BitmapDecoder& bitmap_decoder,
    std::error_code& error_code) {

    auto first_frame = bitmap_decoder.GetFrame(0, error_code);
    if (!IsSucceeded(error_code)) {
        return {};
    }
    return FromBitmap(first_frame);
}


std::shared_ptr<Image> Image::FromStream(const Stream& stream, std::error_code& error_code) {

    auto bitmap_decoder = internal::CreateBitmapDecoderFromSteam(stream, error_code);
    if (!IsSucceeded(error_code)) {
        return {};
    }

    return FromBitmapDecoder(bitmap_decoder, error_code);
}

}