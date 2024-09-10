#pragma once

#include <string>
#include <zaf/control/internal/image_box/image_player.h>

namespace zaf::internal {

class URIImagePlayer : public ImagePlayer {
public:
    URIImagePlayer(const std::wstring& uri, float dpi);

    void GetImageSize(Size& pixel_size, std::pair<float, float>& resolution) override;
    d2d::RenderBitmap GetRenderBitmap(d2d::Renderer& renderer) override;
    void ChangeDPI(float dpi) override;

private:
    void CreateImagePlayerIfNot();

private:
    std::wstring uri_;
    float dpi_{};
    std::unique_ptr<ImagePlayer> inner_;
};

}