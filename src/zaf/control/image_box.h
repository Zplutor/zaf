#pragma once

#include <zaf/control/control.h>
#include <zaf/graphic/image/image_decoder.h>
#include <zaf/graphic/interpolation_mode.h>

namespace zaf {
namespace internal {
class ImagePlayer;
}

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

    std::wstring GetTypeName() const override;

private:
    ImageDecoder image_decoder_;
    std::unique_ptr<internal::ImagePlayer> image_player_;
};

}