#include <zaf/graphic/image/image_decode_frame.h>

namespace zaf {

const ImageMetadataQuerier ImageDecodeFrame::GetMetadataQuerier(std::error_code& error_code) const {

    IWICMetadataQueryReader* handle = nullptr;
    HRESULT result = GetHandle()->GetMetadataQueryReader(&handle);

    error_code = MakeComErrorCode(result);
    return ImageMetadataQuerier(handle);
}

}