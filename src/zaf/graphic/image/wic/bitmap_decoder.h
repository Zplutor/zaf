#pragma once

#include <wincodec.h>
#include <memory>
#include <zaf/base/com_object.h>
#include <zaf/base/error.h>
#include <zaf/graphic/image/wic/container_format.h>
#include <zaf/graphic/image/wic/bitmap_decode_frame.h>
#include <zaf/graphic/image/wic/metadata_query_reader.h>

namespace zaf::wic {

class Palette;

class BitmapDecoder : public ComObject<IWICBitmapDecoder> {
public:
    BitmapDecoder() { }
    explicit BitmapDecoder(IWICBitmapDecoder* handle) : ComObject(handle) { }

    ContainerFormat GetContainerFormat(std::error_code& error_code) const;

    ContainerFormat GetContainerFormat() const {
        std::error_code error_code;
        auto result = GetContainerFormat(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    void CopyPalette(Palette& palette, std::error_code& error_code) const;

    void CopyPalette(Palette& palette) const {
        std::error_code error_code;
        CopyPalette(palette, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    std::size_t GetFrameCount(std::error_code& error_code) const;

    std::size_t GetFrameCount() const {
        std::error_code error_code;
        auto result = GetFrameCount(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    const BitmapDecodeFrame GetFrame(std::size_t index, std::error_code& error_code) const;

    const BitmapDecodeFrame GetFrame(std::size_t index) const {
        std::error_code error_code;
        auto result = GetFrame(index, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    const MetadataQueryReader GetMetadataQuerier(std::error_code& error_code) const;

    const MetadataQueryReader GetMetadataQuerier() const {
        std::error_code error_code;
        auto result = GetMetadataQuerier(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }
};

}