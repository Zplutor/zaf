#pragma once

#include <zaf/base/direct2d.h>

namespace zaf {

class PixelFormat {
public:
    enum class AlphaMode {
        Unknown = D2D1_ALPHA_MODE_UNKNOWN,
        Premultiplied = D2D1_ALPHA_MODE_PREMULTIPLIED,
        Straight = D2D1_ALPHA_MODE_STRAIGHT,
        Ignore = D2D1_ALPHA_MODE_IGNORE,
    };
    
public:
    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
    AlphaMode alpha_mode = AlphaMode::Unknown;
};

class BitmapProperties {
public:
    PixelFormat pixel_format;
    float dpi_x = 0;
    float dpi_y = 0;
};

}