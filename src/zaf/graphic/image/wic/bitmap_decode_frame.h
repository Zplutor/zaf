#pragma once

#include <wincodec.h>
#include <zaf/base/error.h>
#include <zaf/graphic/image/wic/metadata_query_reader.h>
#include <zaf/graphic/image/wic/bitmap_source.h>

namespace zaf::wic {

class BitmapDecodeFrame : public BitmapSource {
public:
    BitmapDecodeFrame() { }
    explicit BitmapDecodeFrame(IWICBitmapFrameDecode* handle) : BitmapSource(handle) { }

    MetadataQueryReader GetMetadataQuerier(std::error_code& error_code) const;

    MetadataQueryReader GetMetadataQuerier() const {
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