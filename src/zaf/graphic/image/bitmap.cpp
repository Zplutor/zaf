#include <zaf/graphic/image/bitmap.h>
#include <zaf/application.h>
#include <zaf/graphic/image/internal/wic_bitmap.h>
#include <zaf/graphic/image/wic/imaging_factory.h>

namespace zaf {

std::shared_ptr<Bitmap> Bitmap::CreateFromFile(
    const std::filesystem::path& file_path,
    std::error_code& error_code) {

    auto bitmap_decoder = GetApplication().GetImagingFactory().CreateDecoderFromFile(
        file_path, 
        error_code);

    if (!IsSucceeded(error_code)) {
        return {};
    }

    return CreateFromBitmapDecoder(bitmap_decoder);
}


std::shared_ptr<Bitmap> Bitmap::CreateFromWicBitmap(const wic::BitmapSource& wic_bitmap) {
    return std::make_shared<internal::WicBitmap>(wic_bitmap);
}


std::shared_ptr<Bitmap> Bitmap::CreateFromBitmapDecoder(
    const wic::BitmapDecoder& bitmap_decoder,
    std::error_code& error_code) {

    auto first_frame = bitmap_decoder.GetFrame(0, error_code);
    if (!IsSucceeded(error_code)) {
        return {};
    }
    return CreateFromWicBitmap(first_frame);
}

}