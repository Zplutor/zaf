#include <zaf/graphic/image/bitmap_image.h>
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


Size BitmapImage::GetPixelSize(std::error_code& error_code) {

    CheckInitialize(error_code);
    if (!IsSucceeded(error_code)) {
        return {};
    }

    return image_->GetPixelSize(error_code);
}


std::pair<float, float> BitmapImage::GetResolution(std::error_code& error_code) {

    CheckInitialize(error_code);
    if (!IsSucceeded(error_code)) {
        return {};
    }

    return image_->GetResolution(error_code);
}


RenderBitmap BitmapImage::CreateRenderBitmap(Renderer& renderer, std::error_code& error_code) {

    CheckInitialize(error_code);
    if (!IsSucceeded(error_code)) {
        return {};
    }

    return image_->CreateRenderBitmap(renderer, error_code);
}


void BitmapImage::CheckInitialize(std::error_code& error_code) {

    if (image_) {
        error_code.clear();
        return;
    }

    auto stream = GetResourceManager().LoadUri(uri_, error_code);
    if (!IsSucceeded(error_code)) {
        return;
    }

    image_ = Image::FromStream(stream, error_code);
}

}