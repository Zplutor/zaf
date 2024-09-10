#include <zaf/graphic/wic/container_format.h>

namespace zaf::wic {

static const struct {
    const ContainerFormat format;
    const GUID& guid;
} g_map[] = {

    { ContainerFormat::Bmp, GUID_ContainerFormatBmp },
    { ContainerFormat::Gif, GUID_ContainerFormatGif },
    { ContainerFormat::Ico, GUID_ContainerFormatIco },
    { ContainerFormat::Jpeg, GUID_ContainerFormatJpeg },
    { ContainerFormat::Png, GUID_ContainerFormatPng },
    { ContainerFormat::Tiff, GUID_ContainerFormatTiff },
    { ContainerFormat::Wmp, GUID_ContainerFormatWmp },
};


ContainerFormat ToContainerFormat(const GUID& container_format_guid) {

    for (const auto& each_item : g_map) {
        if (each_item.guid == container_format_guid) {
            return each_item.format;
        }
    }

    return ContainerFormat::Unknown;
}


const GUID& ToContainerFormatGUID(ContainerFormat container_format) {

    for (const auto& each_item : g_map) {
        if (each_item.format == container_format) {
            return each_item.guid;
        }
    }

    return GUID_NULL;
}

}