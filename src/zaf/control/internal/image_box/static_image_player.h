#pragma once

#include <zaf/graphic/image/image.h>
#include <zaf/control/internal/image_box/image_player.h>

namespace zaf {
namespace internal {

class StaticImagePlayer : public ImagePlayer {
public:
    StaticImagePlayer(const std::shared_ptr<Image>& image) : image_(image) {
    
    }

    RenderBitmap GetRenderBitmap(Renderer& renderer) override {
        return image_->CreateRenderBitmap(renderer);
    }

private:
    std::shared_ptr<Image> image_;
};

}
}