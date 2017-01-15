#include <zaf/control/image_box.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/image/bitmap.h>
#include <zaf/graphic/image/image.h>

namespace zaf {

static const wchar_t* const kInterpolationModePropertyName = L"InterpolationMode";

ImageBox::ImageBox() {

}


ImageBox::~ImageBox() {

}


void ImageBox::Paint(Canvas& canvas, const Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    if (frame_bitmaps_.empty()) {
        if (! CreateFrameBitmaps(canvas.GetRenderer())) {
            return;
        }
    }

    canvas.DrawBitmap(
        frame_bitmaps_[0],
        GetContentRect(),
        DrawImageOptions().InterpolationMode(GetInterpolationMode()));
}


bool ImageBox::CreateFrameBitmaps(const std::shared_ptr<Renderer>& renderer) {

    if (image_ == nullptr) {
        return false;
    }

    for (std::size_t index = 0; index < image_->GetFrameCount(); ++index) {

        auto frame = image_->GetFrame(index);
        if (frame == nullptr) {
            continue;
        }

        auto bitmap = renderer->CreateBitmap(frame);
        if (bitmap == nullptr) {
            continue;
        }

        frame_bitmaps_.push_back(bitmap);
    }

    return ! frame_bitmaps_.empty();
}


void ImageBox::ReleaseRendererResources() {
    frame_bitmaps_.clear();
}


void ImageBox::SetImage(const std::shared_ptr<Image>& image) {

    if (image_ == image) {
        return;
    }

    image_ = image;
    ReleaseRendererResources();
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