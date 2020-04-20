#include <zaf/graphic/image/wic/bitmap_decoder.h>
#include <zaf/graphic/image/wic/palette.h>

namespace zaf::wic {

ContainerFormat BitmapDecoder::GetContainerFormat(std::error_code& error_code) const {

    GUID guid = GUID_NULL;
    HRESULT com_error = GetHandle()->GetContainerFormat(&guid);

    error_code = MakeComErrorCode(com_error);
    return ToContainerFormat(guid);
}


void BitmapDecoder::CopyPalette(Palette& palette, std::error_code& error_code) const {

    HRESULT com_error = GetHandle()->CopyPalette(palette.GetHandle());
    error_code = MakeComErrorCode(com_error);
}


std::size_t BitmapDecoder::GetFrameCount(std::error_code& error_code) const {

    UINT count = 0;
    HRESULT result = GetHandle()->GetFrameCount(&count);

    error_code = MakeComErrorCode(result);
    if (IsSucceeded(error_code)) {
        return count;
    }
    else {
        return 0;
    }
}


const BitmapDecodeFrame BitmapDecoder::GetFrame(std::size_t index, std::error_code& error_code) const {

    IWICBitmapFrameDecode* handle = nullptr;
    HRESULT result = GetHandle()->GetFrame(index, &handle);

    error_code = MakeComErrorCode(result);
    return BitmapDecodeFrame(handle);
}


const MetadataQueryReader BitmapDecoder::GetMetadataQuerier(std::error_code& error_code) const {

    IWICMetadataQueryReader* handle = nullptr;
    HRESULT result = GetHandle()->GetMetadataQueryReader(&handle);

    error_code = MakeComErrorCode(result);
    return MetadataQueryReader(handle);
}

}