#pragma once

#include <optional>
#include <zaf/graphic/size.h>

namespace zaf {

enum class CreateCompatibleRendererFlags {
    None = D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_NONE,
    GdiCompatible = D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_GDI_COMPATIBLE,
};


class CreateCompatibleRendererOptions {
public:
    const std::optional<Size>& DesiredSize() const {
        return desired_size_;
    }

    CreateCompatibleRendererOptions& DesiredSize(const std::optional<Size>& size) {
        desired_size_ = size;
        return *this;
    }

    const std::optional<Size>& DesiredPixelSize() const {
        return desired_pixel_size_;
    }

    CreateCompatibleRendererOptions& DesiredPixelSize(const std::optional<Size>& size) {
        desired_pixel_size_ = size;
        return *this;
    }

    CreateCompatibleRendererFlags Flags() const {
        return flags_;
    }

    CreateCompatibleRendererOptions& Flags(CreateCompatibleRendererFlags flags) {
        flags_ = flags;
        return *this;
    }

private:
    std::optional<Size> desired_size_;
    std::optional<Size> desired_pixel_size_;
    CreateCompatibleRendererFlags flags_ = CreateCompatibleRendererFlags::GdiCompatible;
};

}