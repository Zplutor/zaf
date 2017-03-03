#pragma once

#include <wincodec.h>
#include <zaf/base/error.h>
#include <zaf/graphic/image/image_metadata_querier.h>
#include <zaf/graphic/image/image_source.h>

namespace zaf {

class ImageDecodeFrame : public ImageSource {
public:
    ImageDecodeFrame() { }
    explicit ImageDecodeFrame(IWICBitmapFrameDecode* handle) : ImageSource(handle) { }

    const ImageMetadataQuerier GetMetadataQuerier(std::error_code& error_code) const;

    const ImageMetadataQuerier GetMetadataQuerier() const {
        std::error_code error_code;
        auto result = GetMetadataQuerier(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    IWICBitmapFrameDecode* GetHandle() const {
        return static_cast<IWICBitmapFrameDecode*>(__super::GetHandle());
    }
};

}