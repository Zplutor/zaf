#pragma once

#include <wincodec.h>
#include <memory>
#include <zaf/base/com_object.h>
#include <zaf/base/error.h>
#include <zaf/graphic/image/image_container_format.h>
#include <zaf/graphic/image/image_decode_frame.h>
#include <zaf/graphic/image/image_metadata_querier.h>

namespace zaf {

class ImagePalette;

class ImageDecoder : public ComObject<IWICBitmapDecoder> {
public:
    ImageDecoder() { }
    explicit ImageDecoder(IWICBitmapDecoder* handle) : ComObject(handle) { }

    ImageContainerFormat GetContainerFormat(std::error_code& error_code) const;

    ImageContainerFormat GetContainerFormat() const {
        std::error_code error_code;
        auto result = GetContainerFormat(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    void CopyPalette(ImagePalette& palette, std::error_code& error_code) const;

    void CopyPalette(ImagePalette& palette) const {
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

    const ImageDecodeFrame GetFrame(std::size_t index, std::error_code& error_code) const;

    const ImageDecodeFrame GetFrame(std::size_t index) const {
        std::error_code error_code;
        auto result = GetFrame(index, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    const ImageMetadataQuerier GetMetadataQuerier(std::error_code& error_code) const;

    const ImageMetadataQuerier GetMetadataQuerier() const {
        std::error_code error_code;
        auto result = GetMetadataQuerier(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }
};

}