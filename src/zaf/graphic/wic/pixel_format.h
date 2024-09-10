#pragma once

#include <wincodec.h>

namespace zaf::wic {

enum class PixelFormat {

    Unspecified,

    BGR24,
    BGR32,
    BGRA32,

    RGB24,
    RGBA32,
};

PixelFormat ToPixelFormat(const WICPixelFormatGUID& wic_pixel_format_guid);
const WICPixelFormatGUID& ToWICPixelFormatGUID(PixelFormat image_pixel_format);

}