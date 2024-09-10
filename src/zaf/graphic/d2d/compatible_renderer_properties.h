#pragma once

#include <optional>
#include <zaf/graphic/size.h>

namespace zaf::d2d {

enum class CompatibleRendererOptions {
    None = D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_NONE,
    GDICompatible = D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_GDI_COMPATIBLE,
};


class CompatibleRendererProperties {
public:
    const std::optional<Size>& DesiredSize() const {
        return desired_size_;
    }

    CompatibleRendererProperties& DesiredSize(const std::optional<Size>& size) {
        desired_size_ = size;
        return *this;
    }

    const std::optional<Size>& DesiredPixelSize() const {
        return desired_pixel_size_;
    }

    CompatibleRendererProperties& DesiredPixelSize(const std::optional<Size>& size) {
        desired_pixel_size_ = size;
        return *this;
    }

    CompatibleRendererOptions Options() const {
        return options_;
    }

    CompatibleRendererProperties& Options(CompatibleRendererOptions options) {
        options_ = options;
        return *this;
    }

private:
    std::optional<Size> desired_size_;
    std::optional<Size> desired_pixel_size_;
    CompatibleRendererOptions options_ = CompatibleRendererOptions::GDICompatible;
};

}