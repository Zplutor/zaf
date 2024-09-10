#include <zaf/graphic/wic/bitmap_decoder.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/wic/palette.h>

namespace zaf::wic {

ContainerFormat BitmapDecoder::GetContainerFormat() const {

    GUID guid = GUID_NULL;
    HRESULT com_error = Ptr()->GetContainerFormat(&guid);

    ZAF_THROW_IF_COM_ERROR(com_error);
    return ToContainerFormat(guid);
}


void BitmapDecoder::CopyPalette(Palette& palette) const {

    HRESULT com_error = Ptr()->CopyPalette(palette.Ptr().Inner());
    ZAF_THROW_IF_COM_ERROR(com_error);
}


std::size_t BitmapDecoder::GetFrameCount() const {

    UINT count = 0;
    HRESULT result = Ptr()->GetFrameCount(&count);

    ZAF_THROW_IF_COM_ERROR(result);
    return count;
}


BitmapDecodeFrame BitmapDecoder::GetFrame(std::size_t index) const {

    COMPtr<IWICBitmapFrameDecode> inner;
    HRESULT result = Ptr()->GetFrame(static_cast<UINT>(index), inner.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return BitmapDecodeFrame(inner);
}


MetadataQueryReader BitmapDecoder::GetMetadataQuerier() const {

    COMPtr<IWICMetadataQueryReader> inner;
    HRESULT result = Ptr()->GetMetadataQueryReader(inner.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return MetadataQueryReader(inner);
}

}