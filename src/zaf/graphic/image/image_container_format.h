#pragma once

#include <wincodec.h>

namespace zaf {

enum ImageContainerFormat {
    Unknown,
    Bmp,
    Gif,
    Ico,
    Jpeg,
    Png,
    Tiff,
    Wmp,
};

ImageContainerFormat ToImageContainerFormat(const GUID& container_format_guid);

const GUID& ToImageContainerFormatGUID(ImageContainerFormat container_format);

}