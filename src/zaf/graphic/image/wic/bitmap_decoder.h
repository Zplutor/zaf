#pragma once

#include <wincodec.h>
#include <memory>
#include <zaf/base/com_object.h>
#include <zaf/graphic/image/wic/container_format.h>
#include <zaf/graphic/image/wic/bitmap_decode_frame.h>
#include <zaf/graphic/image/wic/metadata_query_reader.h>

namespace zaf::wic {

class Palette;

class BitmapDecoder : public COMObject<IWICBitmapDecoder> {
public:
    using COMObject::COMObject;

    ContainerFormat GetContainerFormat() const;

    void CopyPalette(Palette& palette) const;

    std::size_t GetFrameCount() const;

    BitmapDecodeFrame GetFrame(std::size_t index) const;

    MetadataQueryReader GetMetadataQuerier() const;
};

}