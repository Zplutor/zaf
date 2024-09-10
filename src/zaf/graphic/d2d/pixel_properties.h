#pragma once

#include <zaf/graphic/d2d/pixel_format.h>

namespace zaf::d2d {

enum class PixelAlphaMode {
    Unknown = D2D1_ALPHA_MODE_UNKNOWN,
    Premultiplied = D2D1_ALPHA_MODE_PREMULTIPLIED,
    Straight = D2D1_ALPHA_MODE_STRAIGHT,
    Ignore = D2D1_ALPHA_MODE_IGNORE,
};

class PixelProperties {
public:
    PixelProperties() = default;
    explicit PixelProperties(const D2D1_PIXEL_FORMAT& inner) : inner_(inner) { }

    PixelFormat Format() const {
        return static_cast<PixelFormat>(inner_.format);
    }

    void SetFormat(PixelFormat value) {
        inner_.format = static_cast<DXGI_FORMAT>(value);
    }

    PixelAlphaMode AlphaMode() const {
        return static_cast<PixelAlphaMode>(inner_.alphaMode);
    }

    void SetAlphaMode(PixelAlphaMode value) {
        inner_.alphaMode = static_cast<D2D1_ALPHA_MODE>(value);
    }

    const D2D1_PIXEL_FORMAT& Inner() const {
        return inner_;
    }

private:
    D2D1_PIXEL_FORMAT inner_{};
};

}