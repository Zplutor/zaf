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
    ZAF_OBJECT;

public:
    ImageBox();
    ~ImageBox();

    void SetImage(const std::shared_ptr<Image>& image);
    void SetDecoder(const wic::BitmapDecoder& decoder);
    void SetFilePath(const std::filesystem::path& file_path);
    void SetURI(const std::wstring& uri);

    ImageLayout ImageLayout() const;
    void SetImageLayout(zaf::ImageLayout image_layout);

    InterpolationMode InterpolationMode() const;
    void SetInterpolationMode(zaf::InterpolationMode mode);

protected:
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const override;
    void ReleaseRendererResources() override;
    void OnDPIChanged() override;

    zaf::Size CalculatePreferredContentSize(const zaf::Size& max_size) const override;

private:
    void SetImagePlayer(std::unique_ptr<internal::ImagePlayer> player);

private:
    std::unique_ptr<internal::ImagePlayer> image_player_;
    zaf::ImageLayout image_layout_{ zaf::ImageLayout::None };
    zaf::InterpolationMode interpolation_mode_{ zaf::InterpolationMode::Linear };
};

ZAF_OBJECT_BEGIN(ImageBox)
ZAF_OBJECT_PROPERTY(ImageLayout)
ZAF_OBJECT_PROPERTY(InterpolationMode)
ZAF_OBJECT_PROPERTY(URI)
ZAF_OBJECT_END;

}