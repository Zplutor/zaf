#pragma once

#include <filesystem>
#include <zaf/control/control.h>
#include <zaf/control/image_layout.h>
#include <zaf/graphic/interpolation_mode.h>

namespace zaf {
namespace wic {
class BitmapDecoder;
}

namespace internal {
class ImagePlayer;
}

class Image;

class ImageBox : public Control {
public:
    ZAF_DECLARE_REFLECTION_TYPE

public:
    ImageBox();
    ~ImageBox();

    void SetImage(const std::shared_ptr<Image>& image);
    void SetDecoder(const wic::BitmapDecoder& decoder);
    void SetFilePath(const std::filesystem::path& file_path);
    void SetUri(const std::wstring& uri);

    ImageLayout GetImageLayout() const;
    void SetImageLayout(ImageLayout image_layout);

    InterpolationMode GetInterpolationMode() const;
    void SetInterpolationMode(InterpolationMode mode);

protected:
    void Paint(Canvas& canvas, const Rect& dirty_rect) override;
    void ReleaseRendererResources() override;

    Size GetPreferredContentSize() const override;

private:
    void SetImagePlayer(std::unique_ptr<internal::ImagePlayer> player);

private:
    std::unique_ptr<internal::ImagePlayer> image_player_;
};

}