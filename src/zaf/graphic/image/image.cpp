#include <zaf/graphic/image/image.h>
#include <zaf/application.h>
#include <zaf/graphic/image/internal/utility.h>
#include <zaf/graphic/image/internal/wic_bitmap.h>
#include <zaf/graphic/image/wic/imaging_factory.h>
#include <zaf/resource/resource_factory.h>

namespace zaf {

std::shared_ptr<Image> Image::FromFile(const std::filesystem::path& file_path) {

    auto bitmap_decoder = wic::ImagingFactory::Instance().CreateDecoderFromFile(file_path);
    return FromBitmapDecoder(bitmap_decoder);
}


std::shared_ptr<Image> Image::FromURI(const std::wstring& uri) {
    return FromStream(ResourceFactory::Instance().LoadURI(uri));
}


std::shared_ptr<Image> Image::FromURI(const std::wstring& uri, float dpi) {
    return FromStream(ResourceFactory::Instance().LoadURI(uri, dpi));
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


ZAF_OBJECT_IMPL(URIImage);


URIImage::URIImage() : dpi_(Application::Instance().GetSystemDPI()) {

}


void URIImage::InitializeWithURI(const std::wstring& uri) {

    ZAF_EXPECT(!image_);
    uri_ = uri;
}


void URIImage::ChangeDPI(float dpi) {

    if (dpi_ != dpi) {
        dpi_ = dpi;
        image_.reset();
    }
}


Size URIImage::GetPixelSize() {

    LoadImageIfNot();
    return image_->GetPixelSize();
}


std::pair<float, float> URIImage::GetResolution() {

    LoadImageIfNot();
    return image_->GetResolution();
}


d2d::RenderBitmap URIImage::CreateRenderBitmap(d2d::Renderer& renderer) {

    LoadImageIfNot();
    return image_->CreateRenderBitmap(renderer);
}


void URIImage::LoadImageIfNot() {

    if (image_) {
        return;
    }

    auto stream = ResourceFactory::Instance().LoadURI(uri_, dpi_);
    image_ = Image::FromStream(stream);
}

}