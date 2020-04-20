#include <zaf/graphic/image/wic/pixel_format.h>

namespace zaf::wic {

static const struct MapItem {
    PixelFormat image_pixel_format;
    const WICPixelFormatGUID& wic_pixel_format_guid;
} g_map[] = {

    PixelFormat::Unspecified, GUID_WICPixelFormatUndefined,
    PixelFormat::BGR24,       GUID_WICPixelFormat24bppBGR,
    PixelFormat::BGR32,       GUID_WICPixelFormat32bppBGR,
    PixelFormat::BGRA32,      GUID_WICPixelFormat32bppBGRA,
    PixelFormat::RGB24,       GUID_WICPixelFormat24bppRGB,
    PixelFormat::RGBA32,      GUID_WICPixelFormat32bppRGBA,
};

PixelFormat ToPixelFormat(const WICPixelFormatGUID& wic_pixel_format_guid) {

    for (const auto& each_item : g_map) {
        if (each_item.wic_pixel_format_guid == wic_pixel_format_guid) {
            return each_item.image_pixel_format;
        }
    }

    return PixelFormat::Unspecified;
}


const WICPixelFormatGUID& ToWICPixelFormatGUID(PixelFormat image_pixel_format) {

    for (const auto& each_item : g_map) {
        if (each_item.image_pixel_format == image_pixel_format) {
            return each_item.wic_pixel_format_guid;
        }
    }

    return GUID_WICPixelFormatUndefined;
}

}