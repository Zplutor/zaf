#pragma once

#include <wincodec.h>

namespace zaf::wic {

enum class ContainerFormat {
    Unknown,
    Bmp,
    Gif,
    Ico,
    Jpeg,
    Png,
    Tiff,
    Wmp,
};

ContainerFormat ToContainerFormat(const GUID& container_format_guid);

const GUID& ToContainerFormatGUID(ContainerFormat container_format);

}