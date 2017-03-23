#pragma once

#include <zaf/graphic/pixel_format.h>

namespace zaf {

class PixelProperties {
public:
    enum class AlphaMode {
        Unknown = D2D1_ALPHA_MODE_UNKNOWN,
        Premultiplied = D2D1_ALPHA_MODE_PREMULTIPLIED,
        Straight = D2D1_ALPHA_MODE_STRAIGHT,
        Ignore = D2D1_ALPHA_MODE_IGNORE,
    };

public:
    PixelFormat format = PixelFormat::Unknown;
    AlphaMode alpha_mode = AlphaMode::Unknown;
};

}