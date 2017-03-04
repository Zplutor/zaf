#include <zaf/graphic/image/image_container_format.h>

namespace zaf {

static const struct {
    const ImageContainerFormat format;
    const GUID& guid;
} g_map[] = {

    { ImageContainerFormat::Bmp, GUID_ContainerFormatBmp },
    { ImageContainerFormat::Gif, GUID_ContainerFormatGif },
    { ImageContainerFormat::Ico, GUID_ContainerFormatIco },
    { ImageContainerFormat::Jpeg, GUID_ContainerFormatJpeg },
    { ImageContainerFormat::Png, GUID_ContainerFormatPng },
    { ImageContainerFormat::Tiff, GUID_ContainerFormatTiff },
    { ImageContainerFormat::Wmp, GUID_ContainerFormatWmp },
};


ImageContainerFormat ToImageContainerFormat(const GUID& container_format_guid) {

    for (const auto& each_item : g_map) {
        if (each_item.guid == container_format_guid) {
            return each_item.format;
        }
    }

    return ImageContainerFormat::Unknown;
}


const GUID& ToImageContainerFormatGUID(ImageContainerFormat container_format) {

    for (const auto& each_item : g_map) {
        if (each_item.format == container_format) {
            return each_item.guid;
        }
    }

    return GUID_NULL;
}

}