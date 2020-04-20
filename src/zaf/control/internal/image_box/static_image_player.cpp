#include <zaf/control/internal/image_box/static_image_player.h>
#include <zaf/graphic/renderer/renderer.h>

namespace zaf {
namespace internal {

const RenderBitmap StaticImagePlayer::GetBitmap(Renderer& renderer) {

    auto frame = image_decoder_.GetFrame(0);
    if (frame == nullptr) {
        return RenderBitmap();
    }

    return renderer.CreateBitmap(frame);
}

}
}