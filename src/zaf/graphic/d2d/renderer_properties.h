#pragma once

#include <zaf/base/direct2d.h>
#include <zaf/graphic/d2d/pixel_properties.h>

namespace zaf::d2d {

enum class RendererType {
    Default = D2D1_RENDER_TARGET_TYPE_DEFAULT,
    Hardware = D2D1_RENDER_TARGET_TYPE_HARDWARE,
    Software = D2D1_RENDER_TARGET_TYPE_SOFTWARE,
};

enum class RendererUsage {
    None = D2D1_RENDER_TARGET_USAGE_NONE,
    ForceBitmapRemoting = D2D1_RENDER_TARGET_USAGE_FORCE_BITMAP_REMOTING,
    GdiCompatible = D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
};

enum class RendererFeatureLevel {
    Default = D2D1_FEATURE_LEVEL_DEFAULT,
    DirectX9 = D2D1_FEATURE_LEVEL_9,
    DirectX10 = D2D1_FEATURE_LEVEL_10,
};


class RendererProperties {
public:
    RendererType Type() const {
        return static_cast<RendererType>(inner_.type);
    }

    void SetType(RendererType value) {
        inner_.type = static_cast<D2D1_RENDER_TARGET_TYPE>(value);
    }

    RendererUsage Usage() const {
        return static_cast<RendererUsage>(inner_.usage);
    }

    void SetUsage(RendererUsage value) {
        inner_.usage = static_cast<D2D1_RENDER_TARGET_USAGE>(value);
    }

    void SetPixelProperties(const PixelProperties& value) {
        inner_.pixelFormat = value.Inner();
    }

    float DPIX() const {
        return inner_.dpiX;
    }

    void SetDPIX(float value) {
        inner_.dpiX = value;
    }

    float DPIY() const {
        return inner_.dpiY;
    }

    void SetDPIY(float value) {
        inner_.dpiY = value;
    }

    RendererFeatureLevel MinFeatureLevel() const {
        return static_cast<RendererFeatureLevel>(inner_.minLevel);
    }

    void SetMinFeatureLevel(RendererFeatureLevel value) {
        inner_.minLevel = static_cast<D2D1_FEATURE_LEVEL>(value);
    }

    const D2D1_RENDER_TARGET_PROPERTIES& Inner() const {
        return inner_;
    }

private:
    D2D1_RENDER_TARGET_PROPERTIES inner_{};
};

}