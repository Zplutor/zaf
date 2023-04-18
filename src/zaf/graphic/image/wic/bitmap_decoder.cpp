#include <zaf/graphic/image/wic/bitmap_decoder.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/image/wic/palette.h>

namespace zaf::wic {

ContainerFormat BitmapDecoder::GetContainerFormat() const {

    GUID guid = GUID_NULL;
    HRESULT com_error = Inner()->GetContainerFormat(&guid);

    ZAF_THROW_IF_COM_ERROR(com_error);
    return ToContainerFormat(guid);
}


void BitmapDecoder::CopyPalette(Palette& palette) const {

    HRESULT com_error = Inner()->CopyPalette(palette.Inner());
    ZAF_THROW_IF_COM_ERROR(com_error);
}


std::size_t BitmapDecoder::GetFrameCount() const {

    UINT count = 0;
    HRESULT result = Inner()->GetFrameCount(&count);

    ZAF_THROW_IF_COM_ERROR(result);
    return count;
}


BitmapDecodeFrame BitmapDecoder::GetFrame(std::size_t index) const {

    IWICBitmapFrameDecode* handle = nullptr;
    HRESULT result = Inner()->GetFrame(static_cast<UINT>(index), &handle);

    ZAF_THROW_IF_COM_ERROR(result);
    return BitmapDecodeFrame(handle);
}


MetadataQueryReader BitmapDecoder::GetMetadataQuerier() const {

    IWICMetadataQueryReader* handle = nullptr;
    HRESULT result = Inner()->GetMetadataQueryReader(&handle);

    ZAF_THROW_IF_COM_ERROR(result);
    return MetadataQueryReader(handle);
}

}