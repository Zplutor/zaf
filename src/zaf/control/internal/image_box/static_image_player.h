#pragma once

#include <zaf/graphic/image/image.h>
#include <zaf/control/internal/image_box/image_player.h>

namespace zaf {
namespace internal {

class StaticImagePlayer : public ImagePlayer {
public:
    StaticImagePlayer(const std::shared_ptr<Image>& image) : image_(image) {
    
    }

    void GetImageSize(Size& pixel_size, std::pair<float, float>& resolution) override {
        pixel_size = image_->GetPixelSize();
        resolution = image_->GetResolution();
    }

    d2d::RenderBitmap GetRenderBitmap(d2d::Renderer& renderer) override {
        return image_->CreateRenderBitmap(renderer);
    }

private:
    std::shared_ptr<Image> image_;
};

}
}