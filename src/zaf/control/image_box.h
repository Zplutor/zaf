#pragma once

#include <zaf/control/control.h>
#include <zaf/graphic/image/interpolation_mode.h>

namespace zaf {

class Bitmap;
class ImageDecoder;

class ImageBox : public Control {
public:
    ImageBox();
    ~ImageBox();

    const std::shared_ptr<ImageDecoder>& GetImageDecoder() const {
        return image_decoder_;
    }

    void SetImageDecoder(const std::shared_ptr<ImageDecoder>& image_decoder);

    InterpolationMode GetInterpolationMode() const;
    void SetInterpolationMode(InterpolationMode mode);

protected:
    void Paint(Canvas& canvas, const Rect& dirty_rect) override;
    void ReleaseRendererResources() override;

private:
    bool CreateFrameBitmaps(const std::shared_ptr<Renderer>& renderer);

private:
    std::shared_ptr<ImageDecoder> image_decoder_;
    std::vector<std::shared_ptr<Bitmap>> frame_bitmaps_;
};

}