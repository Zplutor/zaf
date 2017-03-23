#include <zaf/graphic/image/image_pixel_format.h>

namespace zaf {

static const struct MapItem {
    ImagePixelFormat image_pixel_format;
    const WICPixelFormatGUID& wic_pixel_format_guid;
} g_map[] = {

    ImagePixelFormat::Unspecified, GUID_WICPixelFormatUndefined,
    ImagePixelFormat::BGR24,       GUID_WICPixelFormat24bppBGR,
    ImagePixelFormat::BGR32,       GUID_WICPixelFormat32bppBGR,
    ImagePixelFormat::BGRA32,      GUID_WICPixelFormat32bppBGRA,
    ImagePixelFormat::RGB24,       GUID_WICPixelFormat24bppRGB,
    ImagePixelFormat::RGBA32,      GUID_WICPixelFormat32bppRGBA,
};

ImagePixelFormat ToImagePixelFormat(const WICPixelFormatGUID& wic_pixel_format_guid) {

    for (const auto& each_item : g_map) {
        if (each_item.wic_pixel_format_guid == wic_pixel_format_guid) {
            return each_item.image_pixel_format;
        }
    }

    return ImagePixelFormat::Unspecified;
}


const WICPixelFormatGUID& ToWICPixelFormatGUID(ImagePixelFormat image_pixel_format) {

    for (const auto& each_item : g_map) {
        if (each_item.image_pixel_format == image_pixel_format) {
            return each_item.wic_pixel_format_guid;
        }
    }

    return GUID_WICPixelFormatUndefined;
}

}