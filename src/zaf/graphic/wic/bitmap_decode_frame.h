#pragma once

#include <wincodec.h>
#include <zaf/graphic/wic/metadata_query_reader.h>
#include <zaf/graphic/wic/bitmap_source.h>

namespace zaf::wic {

class BitmapDecodeFrame : public BitmapSource {
public:
    BitmapDecodeFrame() = default;
    explicit BitmapDecodeFrame(COMPtr<IWICBitmapFrameDecode> ptr) :
        BitmapSource(ptr), 
        inner_(std::move(ptr)) {

    }

    MetadataQueryReader GetMetadataQuerier() const;

    const COMPtr<IWICBitmapFrameDecode>& Inner() const noexcept {
        return inner_;
    }

private:
    COMPtr<IWICBitmapFrameDecode> inner_;
};

}