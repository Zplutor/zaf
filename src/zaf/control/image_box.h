#pragma once

#include <zaf/control/control.h>
#include <zaf/graphic/image/wic/bitmap_decoder.h>
#include <zaf/graphic/interpolation_mode.h>

namespace zaf {
namespace internal {
class ImagePlayer;
}

class ImageBox : public Control {
public:
    ZAF_DECLARE_REFLECTION_TYPE();

public:
    ImageBox();
    ~ImageBox();

    const wic::BitmapDecoder& GetImageDecoder() const {
        return image_decoder_;
    }

    void SetImageDecoder(const wic::BitmapDecoder& image_decoder);

    InterpolationMode GetInterpolationMode() const;
    void SetInterpolationMode(InterpolationMode mode);

protected:
    void Paint(Canvas& canvas, const Rect& dirty_rect) override;
    void ReleaseRendererResources() override;

private:
    wic::BitmapDecoder image_decoder_;
    std::unique_ptr<internal::ImagePlayer> image_player_;
};

}