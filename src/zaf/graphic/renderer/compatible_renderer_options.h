#pragma once

#include <zaf/graphic/size.h>

namespace zaf {

enum class CompatibleRendererFlags {
    None = D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_NONE,
    GdiCompatible = D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_GDI_COMPATIBLE,
};


class CompatibleRendererOptions {
public:
    const Size* DesiredSize() const {
        return has_desired_size_ ? &desired_size_ : nullptr;
    }

    CompatibleRendererOptions& DesiredSize(const Size& size) {
        desired_size_ = size;
        return *this;
    }

    const Size* DesiredPixelSize() const {
        return has_desired_pixel_size_ ? &desired_pixel_size_ : nullptr;
    }

    CompatibleRendererOptions& DesiredPixelSize(const Size& size) {
        desired_pixel_size_ = size;
        return *this;
    }

    CompatibleRendererFlags Flags() const {
        return flags_;
    }

    CompatibleRendererOptions& Flags(CompatibleRendererFlags flags) {
        flags_ = flags;
        return *this;
    }

private:
    bool has_desired_size_ = false;
    Size desired_size_;

    bool has_desired_pixel_size_ = false;
    Size desired_pixel_size_;

    CompatibleRendererFlags flags_ = CompatibleRendererFlags::GdiCompatible;
};

}