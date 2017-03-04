#include <zaf/control/internal/image_box/static_image_player.h>
#include <zaf/graphic/renderer.h>

namespace zaf {
namespace internal {

const Bitmap StaticImagePlayer::GetBitmap(Renderer& renderer) {

    auto frame = image_decoder_.GetFrame(0);
    if (frame == nullptr) {
        return Bitmap();
    }

    return renderer.CreateBitmap(frame);
}

}
}