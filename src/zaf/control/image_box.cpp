#include <zaf/control/image_box.h>
#include <zaf/control/internal/image_box/gif_player.h>
#include <zaf/control/internal/image_box/static_image_player.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

static std::unique_ptr<internal::ImagePlayer> CreateImagePlayer(const ImageDecoder& image_decoder);

static const wchar_t* const kInterpolationModePropertyName = L"InterpolationMode";

ImageBox::ImageBox() {

}


ImageBox::~ImageBox() {

}


void ImageBox::Paint(Canvas& canvas, const Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    if (image_player_ == nullptr) {
        return;
    }

    auto bitmap = image_player_->GetBitmap(canvas.GetRenderer());
    if (bitmap == nullptr) {
        return;
    }

    canvas.DrawBitmap(
        bitmap,
        GetContentRect(),
        DrawImageOptions().InterpolationMode(GetInterpolationMode()));
}


void ImageBox::ReleaseRendererResources() {

    __super::ReleaseRendererResources();

    if (image_player_ != nullptr) {
        image_player_->Reset();
    }
}


void ImageBox::SetImageDecoder(const ImageDecoder& image_decoder) {

    if (image_decoder_ == image_decoder) {
        return;
    }

    image_decoder_ = image_decoder;
    image_player_ = CreateImagePlayer(image_decoder_);
    image_player_->SetUpdateEvent(std::bind(&ImageBox::NeedRepaint, this));
    NeedRepaint();
}


InterpolationMode ImageBox::GetInterpolationMode() const {

    auto mode = GetPropertyMap().TryGetProperty<InterpolationMode>(kInterpolationModePropertyName);
    if (mode != nullptr) {
        return *mode;
    }
    else {
        return InterpolationMode::Linear;
    }
}


void ImageBox::SetInterpolationMode(InterpolationMode mode) {

    GetPropertyMap().SetProperty(kInterpolationModePropertyName, mode);
    NeedRepaint();
}


static std::unique_ptr<internal::ImagePlayer> CreateImagePlayer(const ImageDecoder& image_decoder) {

    auto container_format = image_decoder.GetContainerFormat();
    switch (container_format) {
    case ImageContainerFormat::Gif:
        return std::make_unique<internal::GifPlayer>(image_decoder);
    default:
        return std::make_unique<internal::StaticImagePlayer>(image_decoder);
    }
}


}