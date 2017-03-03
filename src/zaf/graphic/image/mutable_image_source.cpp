#include <zaf/graphic/image/mutable_image_source.h>
#include <zaf/graphic/image/image_palette.h>

namespace zaf {

void MutableImageSource::SetResolution(double x, double y, std::error_code& error_code) {

    HRESULT com_error = GetHandle()->SetResolution(x, y);
    error_code = MakeComErrorCode(com_error);
}


void MutableImageSource::SetPalette(const ImagePalette& palette, std::error_code& error_code) {

    HRESULT com_error = GetHandle()->SetPalette(palette.GetHandle());
    error_code = MakeComErrorCode(com_error);
}


const MutableImageSource::Lock MutableImageSource::GetLock(
    const Rect& rect,
    LockFlag flags, 
    std::error_code& error_code) {

    auto wic_rect = rect.ToWICRect();
    IWICBitmapLock* handle = nullptr;
    HRESULT com_error = GetHandle()->Lock(&wic_rect, static_cast<DWORD>(flags), &handle);

    error_code = MakeComErrorCode(com_error);
    return Lock(handle);
}


const Size MutableImageSource::Lock::GetSize(std::error_code& error_code) const {

    UINT width = 0;
    UINT height = 0;
    HRESULT com_error = GetHandle()->GetSize(&width, &height);

    error_code = MakeComErrorCode(com_error);
    return Size(static_cast<float>(width), static_cast<float>(height));
}


std::uint32_t MutableImageSource::Lock::GetStride(std::error_code& error_code) const {

    UINT stride = 0;
    HRESULT com_error = GetHandle()->GetStride(&stride);

    error_code = MakeComErrorCode(com_error);
    return stride;
}


void MutableImageSource::Lock::GetDataPointer(
    std::uint8_t*& data_pointer,
    std::size_t& data_size,
    std::error_code& error_code) const {

    HRESULT com_error = GetHandle()->GetDataPointer(&data_size, &data_pointer);
    error_code = MakeComErrorCode(com_error);
}

}