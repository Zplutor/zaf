#pragma once

#include <zaf/graphic/image/image_decoder.h>
#include <zaf/control/internal/image_box/image_player.h>

namespace zaf {
namespace internal {

class GifPlayer : public ImagePlayer {
public:
    GifPlayer(const ImageDecoder& image_decoder) : image_decoder_(image_decoder) { }

    const Bitmap GetBitmap(Renderer& renderer) override;

private:
    ImageDecoder image_decoder_;
};

}
}