#include <zaf/graphic/image/image.h>
#include <zaf/application.h>
#include <zaf/graphic/image/internal/utility.h>
#include <zaf/graphic/image/internal/wic_bitmap.h>
#include <zaf/graphic/image/wic/imaging_factory.h>
#include <zaf/resource/resource_manager.h>

namespace zaf {

std::shared_ptr<Image> Image::FromFile(const std::filesystem::path& file_path) {

    auto bitmap_decoder = GetApplication().GetImagingFactory().CreateDecoderFromFile(file_path);
    return FromBitmapDecoder(bitmap_decoder);
}


std::shared_ptr<Image> Image::FromURI(const std::wstring& uri) {
    return FromStream(GetResourceManager().LoadURI(uri));
}


std::shared_ptr<Image> Image::FromURI(const std::wstring& uri, float dpi) {
    return FromStream(GetResourceManager().LoadURI(uri, dpi));
}


std::shared_ptr<Image> Image::FromBitmap(const wic::BitmapSource& wic_bitmap) {
    return std::make_shared<internal::WicBitmap>(wic_bitmap);
}


std::shared_ptr<Image> Image::FromBitmapDecoder(const wic::BitmapDecoder& bitmap_decoder) {

    auto first_frame = bitmap_decoder.GetFrame(0);
    return FromBitmap(first_frame);
}


std::shared_ptr<Image> Image::FromStream(const Stream& stream) {

    auto bitmap_decoder = internal::CreateBitmapDecoderFromSteam(stream);
    return FromBitmapDecoder(bitmap_decoder);
}

}