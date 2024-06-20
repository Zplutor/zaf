#include <zaf/graphic/image/uri_image.h>
#include <zaf/application.h>
#include <zaf/resource/resource_factory.h>

namespace zaf {

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


RenderBitmap URIImage::CreateRenderBitmap(Renderer& renderer) {

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