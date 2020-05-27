#include <zaf/graphic/image/bitmap_image.h>
#include <zaf/base/assert.h>
#include <zaf/parsing/parsers/bitmap_image_parser.h>
#include <zaf/reflection/reflection_type_definition.h>
#include <zaf/resource/resource_manager.h>

namespace zaf {

ZAF_DEFINE_REFLECTION_TYPE(BitmapImage, ReflectionObject);


void BitmapImage::SetUri(const std::wstring& uri) {

    if (!image_) {
        uri_ = uri;
    }
    else {
        ZAF_FAIL();
    }
}


Size BitmapImage::GetPixelSize() {

    CheckInitialize();
    return image_->GetPixelSize();
}


std::pair<float, float> BitmapImage::GetResolution() {

    CheckInitialize();
    return image_->GetResolution();
}


RenderBitmap BitmapImage::CreateRenderBitmap(Renderer& renderer) {

    CheckInitialize();
    return image_->CreateRenderBitmap(renderer);
}


void BitmapImage::CheckInitialize() {

    if (image_) {
        return;
    }

    auto stream = GetResourceManager().LoadUri(uri_);
    image_ = Image::FromStream(stream);
}

}