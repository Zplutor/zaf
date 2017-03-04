#include <zaf/graphic/image/image_decoder.h>

namespace zaf {

ImageContainerFormat ImageDecoder::GetContainerFormat(std::error_code& error_code) const {

    GUID guid = GUID_NULL;
    HRESULT com_error = GetHandle()->GetContainerFormat(&guid);

    error_code = MakeComErrorCode(com_error);
    return ToImageContainerFormat(guid);
}


std::size_t ImageDecoder::GetFrameCount(std::error_code& error_code) const {

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


const ImageDecodeFrame ImageDecoder::GetFrame(std::size_t index, std::error_code& error_code) const {

    IWICBitmapFrameDecode* handle = nullptr;
    HRESULT result = GetHandle()->GetFrame(index, &handle);

    error_code = MakeComErrorCode(result);
    return ImageDecodeFrame(handle);
}


const ImageMetadataQuerier ImageDecoder::GetMetadataQuerier(std::error_code& error_code) const {

    IWICMetadataQueryReader* handle = nullptr;
    HRESULT result = GetHandle()->GetMetadataQueryReader(&handle);

    error_code = MakeComErrorCode(result);
    return ImageMetadataQuerier(handle);
}

}