#include <zaf/graphic/image/wic/bitmap_decode_frame.h>
#include <zaf/base/error/com_error.h>

namespace zaf::wic {

MetadataQueryReader BitmapDecodeFrame::GetMetadataQuerier() const {

    IWICMetadataQueryReader* handle = nullptr;
    HRESULT result = GetHandle()->GetMetadataQueryReader(&handle);

    ZAF_THROW_IF_COM_ERROR(result);
    return MetadataQueryReader(handle);
}

}