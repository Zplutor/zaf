#pragma once

#include <zaf/graphic/pixel_properties.h>

namespace zaf {

class BitmapProperties {
public:
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

    const D2D1_BITMAP_PROPERTIES& Inner() const {
        return inner_;
    }

private:
    D2D1_BITMAP_PROPERTIES inner_{};
};

}