#pragma once

#include <wincodec.h>

namespace zaf {

enum class ImagePixelFormat {

    Unspecified,

    BGR24,
    BGR32,
    BGRA32,

    RGB24,
    RGBA32,
};

ImagePixelFormat ToImagePixelFormat(const WICPixelFormatGUID& wic_pixel_format_guid);
const WICPixelFormatGUID& ToWICPixelFormatGUID(ImagePixelFormat image_pixel_format);

}