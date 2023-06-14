#include <zaf/graphic/image/wic/bitmap_decode_frame.h>
#include <zaf/base/error/com_error.h>

namespace zaf::wic {

MetadataQueryReader BitmapDecodeFrame::GetMetadataQuerier() const {

    COMPtr<IWICMetadataQueryReader> ptr;
    HRESULT result = Inner()->GetMetadataQueryReader(ptr.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return MetadataQueryReader(ptr);
}

}