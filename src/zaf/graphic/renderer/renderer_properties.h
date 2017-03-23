#pragma once

#include <zaf/base/direct2d.h>
#include <zaf/graphic/pixel_properties.h>

namespace zaf {

class RendererProperties {
public:
    enum class Type {
        Default = D2D1_RENDER_TARGET_TYPE_DEFAULT,
        Hardware = D2D1_RENDER_TARGET_TYPE_HARDWARE,
        Software = D2D1_RENDER_TARGET_TYPE_SOFTWARE,
    };

    enum class Usage {
        None = D2D1_RENDER_TARGET_USAGE_NONE,
        ForceBitmapRemoting = D2D1_RENDER_TARGET_USAGE_FORCE_BITMAP_REMOTING,
        GdiCompatible = D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
    };

    enum class FeatureLevel {
        Default = D2D1_FEATURE_LEVEL_DEFAULT,
        DirectX9 = D2D1_FEATURE_LEVEL_9,
        DirectX10 = D2D1_FEATURE_LEVEL_10,
    };

public:
    Type type = Type::Default;
    Usage usage = Usage::None;
    PixelProperties pixel_properties;
    float dpi_x = 0;
    float dpi_y = 0;
    FeatureLevel feature_level = FeatureLevel::Default;
};

}