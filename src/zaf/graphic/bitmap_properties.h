#pragma once

#include <zaf/graphic/pixel_properties.h>

namespace zaf {

class BitmapProperties {
public:
    PixelProperties pixel_properties;
    float dpi_x = 0;
    float dpi_y = 0;
};

}