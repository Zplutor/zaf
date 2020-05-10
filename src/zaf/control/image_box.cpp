#include <zaf/control/image_box.h>
#include <zaf/application.h>
#include <zaf/graphic/image/image.h>
#include <zaf/graphic/image/internal/utility.h>
#include <zaf/graphic/image/wic/imaging_factory.h>
#include <zaf/control/internal/image_box/gif_player.h>
#include <zaf/control/internal/image_box/image_drawing.h>
#include <zaf/control/internal/image_box/static_image_player.h>
#include <zaf/graphic/canvas.h>
#include <zaf/parsing/parsers/image_box_parser.h>
#include <zaf/reflection/reflection_type_definition.h>
#include <zaf/resource/resource_manager.h>

namespace zaf {
namespace {

std::unique_ptr<internal::ImagePlayer> CreateImagePlayer(const wic::BitmapDecoder& bitmap_decoder) {

    auto container_format = bitmap_decoder.GetContainerFormat();
    switch (container_format) {

        case wic::ContainerFormat::Gif:
            return std::make_unique<internal::GifPlayer>(bitmap_decoder);

        default: {
            auto bitmap = Image::FromBitmapDecoder(bitmap_decoder);
            if (!bitmap) {
                return {};
            }
            return std::make_unique<internal::StaticImagePlayer>(bitmap);
        }
    }
}

const wchar_t* const kImageLayoutPropertyName = L"ImageLayout";
const wchar_t* const kInterpolationModePropertyName = L"InterpolationMode";

}


ZAF_DEFINE_REFLECTION_TYPE(ImageBox, Control);


ImageBox::ImageBox() {

}


ImageBox::~ImageBox() {

}


void ImageBox::Paint(Canvas& canvas, const Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    if (image_player_ == nullptr) {
        return;
    }

    auto bitmap = image_player_->GetRenderBitmap(canvas.GetRenderer());
    if (bitmap == nullptr) {
        return;
    }

    auto content_rect = GetContentRect();

    canvas.PushClippingRect(content_rect);
    internal::DrawImage(canvas, content_rect, GetImageLayout(), bitmap, GetInterpolationMode());
    canvas.PopClippingRect();
}


void ImageBox::ReleaseRendererResources() {

    __super::ReleaseRendererResources();

    if (image_player_ != nullptr) {
        image_player_->Reset();
    }
}


void ImageBox::SetImage(const std::shared_ptr<Image>& image) {

    image_player_ = std::make_unique<internal::StaticImagePlayer>(image);
    NeedRepaint();
}


void ImageBox::SetDecoder(const wic::BitmapDecoder& decoder) {

    image_player_ = CreateImagePlayer(decoder);
    image_player_->SetUpdateEvent(std::bind(&ImageBox::NeedRepaint, this));
    NeedRepaint();
}


void ImageBox::SetFilePath(const std::filesystem::path& file_path) {

    auto decoder = GetImagingFactory().CreateDecoderFromFile(file_path);
    if (decoder == nullptr) {
        return;
    }

    SetDecoder(decoder);
}


void ImageBox::SetUri(const std::wstring& uri) {

    auto stream = GetResourceManager().LoadUri(uri);
    if (stream == nullptr) {
        return;
    }

    std::error_code error_code;
    auto bitmap_decoder = internal::CreateBitmapDecoderFromSteam(stream, error_code);
    if (bitmap_decoder == nullptr) {
        return;
    }

    SetDecoder(bitmap_decoder);
}


Size ImageBox::GetPreferredContentSize() const {

    Size pixel_size;
    std::pair<float, float> resolution;
    image_player_->GetImageSize(pixel_size, resolution);

    if (resolution.first == 0 || resolution.second == 0) {
        return {};
    }

    return Size{
        pixel_size.width / (std::round(resolution.first) / 96.f),
        pixel_size.height / (std::round(resolution.second) / 96.f)
    };
}


ImageLayout ImageBox::GetImageLayout() const {

    auto image_layout = GetPropertyMap().TryGetProperty<ImageLayout>(kImageLayoutPropertyName);
    if (image_layout) {
        return *image_layout;
    }
    else {
        return ImageLayout::None;
    }
}

void ImageBox::SetImageLayout(ImageLayout image_layout) {

    GetPropertyMap().SetProperty(kImageLayoutPropertyName, image_layout);
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

}