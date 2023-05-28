#include <zaf/control/image_box.h>
#include <zaf/control/internal/image_box/image_drawing.h>
#include <zaf/control/internal/image_box/image_player_factory.h>
#include <zaf/graphic/canvas.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(ImageBox)
ZAF_DEFINE_TYPE_PROPERTY(ImageLayout)
ZAF_DEFINE_TYPE_PROPERTY(InterpolationMode)
ZAF_DEFINE_TYPE_PROPERTY(URI)
ZAF_DEFINE_TYPE_END


ImageBox::ImageBox() {

}


ImageBox::~ImageBox() {

}


void ImageBox::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    if (!image_player_) {
        return;
    }

    auto bitmap = image_player_->GetRenderBitmap(canvas.Renderer());
    if (bitmap == nullptr) {
        return;
    }

    auto content_rect = ContentRect();
    auto clipping_guard = canvas.PushClipping(content_rect);
    internal::DrawImage(canvas, content_rect, ImageLayout(), bitmap, InterpolationMode());
}


void ImageBox::ReleaseRendererResources() {

    __super::ReleaseRendererResources();

    if (image_player_) {
        image_player_->Reset();
    }
}


void ImageBox::OnDPIChanged() {

    __super::OnDPIChanged();

    if (image_player_) {
        image_player_->ChangeDPI(GetDPI());
    }
}


void ImageBox::SetImage(const std::shared_ptr<Image>& image) {
    SetImagePlayer(internal::ImagePlayerFactory::CreateWithImage(image));
}


void ImageBox::SetDecoder(const wic::BitmapDecoder& decoder) {
    SetImagePlayer(internal::ImagePlayerFactory::CreateWithDecoder(decoder));
}


void ImageBox::SetFilePath(const std::filesystem::path& file_path) {
    SetImagePlayer(internal::ImagePlayerFactory::CreateWithFilePath(file_path));
}


void ImageBox::SetURI(const std::wstring& uri) {
    SetImagePlayer(internal::ImagePlayerFactory::CreateWithURI(uri, GetDPI()));
}


void ImageBox::SetImagePlayer(std::unique_ptr<internal::ImagePlayer> player) {

    image_player_ = std::move(player);
    image_player_->SetUpdateEvent(std::bind(&ImageBox::NeedRepaint, this));
    NeedRepaint();

    RaiseContentChangedEvent();
}


zaf::Size ImageBox::CalculatePreferredContentSize(const zaf::Size& max_size) const {

    if (!image_player_) {
        return {};
    }

    zaf::Size pixel_size;
    std::pair<float, float> resolution;
    image_player_->GetImageSize(pixel_size, resolution);

    if (resolution.first == 0 || resolution.second == 0) {
        return {};
    }

    //TODO: Consider max_size.
    return zaf::Size{
        pixel_size.width / (std::round(resolution.first) / 96.f),
        pixel_size.height / (std::round(resolution.second) / 96.f)
    };
}


ImageLayout ImageBox::ImageLayout() const {
    return image_layout_;
}

void ImageBox::SetImageLayout(zaf::ImageLayout image_layout) {

    image_layout_ = image_layout;
    NeedRepaint();
}


InterpolationMode ImageBox::InterpolationMode() const {
    return interpolation_mode_;
}

void ImageBox::SetInterpolationMode(zaf::InterpolationMode mode) {

    interpolation_mode_ = mode;
    NeedRepaint();
}

}