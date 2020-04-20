#pragma once

#include <zaf/graphic/image/wic/bitmap_decoder.h>
#include <zaf/control/internal/image_box/image_player.h>

namespace zaf {
namespace internal {

class StaticImagePlayer : public ImagePlayer {
public:
    StaticImagePlayer(const wic::BitmapDecoder& image_decoder) : image_decoder_(image_decoder) {
    
    }

    const RenderBitmap GetBitmap(Renderer& renderer) override;

private:
    wic::BitmapDecoder image_decoder_;
};

}
}