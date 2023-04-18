#pragma once

#include <wincodec.h>
#include <zaf/graphic/image/wic/metadata_query_reader.h>
#include <zaf/graphic/image/wic/bitmap_source.h>

namespace zaf::wic {

class BitmapDecodeFrame : public BitmapSource {
public:
    BitmapDecodeFrame() { }
    explicit BitmapDecodeFrame(IWICBitmapFrameDecode* handle) : BitmapSource(handle) { }

    MetadataQueryReader GetMetadataQuerier() const;

    IWICBitmapFrameDecode* Inner() const {
        return static_cast<IWICBitmapFrameDecode*>(__super::Inner());
    }
};

}