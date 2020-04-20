#include <zaf/graphic/image/wic/bitmap_decode_frame.h>

namespace zaf::wic {

MetadataQueryReader BitmapDecodeFrame::GetMetadataQuerier(std::error_code& error_code) const {

    IWICMetadataQueryReader* handle = nullptr;
    HRESULT result = GetHandle()->GetMetadataQueryReader(&handle);

    error_code = MakeComErrorCode(result);
    return MetadataQueryReader(handle);
}

}