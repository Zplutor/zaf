#include <zaf/control/internal/image_box/uri_image_player.h>
#include <zaf/application.h>
#include <zaf/control/internal/image_box/image_player_factory.h>
#include <zaf/graphic/image/internal/utility.h>
#include <zaf/resource/resource_manager.h>

namespace zaf::internal {

URIImagePlayer::URIImagePlayer(const std::wstring& uri, float dpi) : uri_(uri), dpi_(dpi) {

}


void URIImagePlayer::GetImageSize(Size& pixel_size, std::pair<float, float>& resolution) {

    CreateImagePlayerIfNot();
    inner_->GetImageSize(pixel_size, resolution);
}


RenderBitmap URIImagePlayer::GetRenderBitmap(Renderer& renderer) {

    CreateImagePlayerIfNot();
    return inner_->GetRenderBitmap(renderer);
}


void URIImagePlayer::ChangeDPI(float dpi) {

    if (dpi_ != dpi) {
        dpi_ = dpi;
        inner_.reset();
        NotifyUpdate();
    }
}


void URIImagePlayer::CreateImagePlayerIfNot() {

    if (inner_) {
        return;
    }

    auto stream = GetResourceManager().LoadURI(uri_, dpi_);
    auto decoder = internal::CreateBitmapDecoderFromSteam(stream);
    inner_ = ImagePlayerFactory::CreateWithDecoder(decoder);
    inner_->SetUpdateEvent([this]() {
        NotifyUpdate();
    });
}

}