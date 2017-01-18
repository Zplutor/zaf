#pragma once

#include <zaf/control/control.h>
#include <zaf/graphic/image/bitmap.h>
#include <zaf/graphic/image/image_decoder.h>
#include <zaf/graphic/image/interpolation_mode.h>

namespace zaf {

class ImageBox : public Control {
public:
    ImageBox();
    ~ImageBox();

    const ImageDecoder& GetImageDecoder() const {
        return image_decoder_;
    }

    void SetImageDecoder(const ImageDecoder& image_decoder);

    InterpolationMode GetInterpolationMode() const;
    void SetInterpolationMode(InterpolationMode mode);

protected:
    void Paint(Canvas& canvas, const Rect& dirty_rect) override;
    void ReleaseRendererResources() override;

private:
    bool CreateFrameBitmaps(Renderer& renderer);

private:
    ImageDecoder image_decoder_;
    std::vector<Bitmap> frame_bitmaps_;
};

}