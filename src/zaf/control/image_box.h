#pragma once

#include <zaf/control/control.h>
#include <zaf/graphic/image/interpolation_mode.h>

namespace zaf {

class Bitmap;
class Image;

class ImageBox : public Control {
public:
    ImageBox();
    ~ImageBox();

    const std::shared_ptr<Image>& GetImage() const {
        return image_;
    }

    void SetImage(const std::shared_ptr<Image>& image);

    InterpolationMode GetInterpolationMode() const;
    void SetInterpolationMode(InterpolationMode mode);

protected:
    void Paint(Canvas& canvas, const Rect& dirty_rect) override;
    void ReleaseRendererResources() override;

private:
    bool CreateFrameBitmaps(const std::shared_ptr<Renderer>& renderer);

private:
    std::shared_ptr<Image> image_;
    std::vector<std::shared_ptr<Bitmap>> frame_bitmaps_;
};

}