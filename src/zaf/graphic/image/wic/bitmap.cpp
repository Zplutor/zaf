#include <zaf/graphic/image/wic/bitmap.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/image/wic/palette.h>

namespace zaf::wic {

void Bitmap::SetResolution(double x, double y) {

    HRESULT com_error = GetHandle()->SetResolution(x, y);
    ZAF_THROW_IF_COM_ERROR(com_error);
}


void Bitmap::SetPalette(const Palette& palette) {

    HRESULT com_error = GetHandle()->SetPalette(palette.GetHandle());
    ZAF_THROW_IF_COM_ERROR(com_error);
}


Bitmap::Lock Bitmap::GetLock(const Rect& rect, LockFlag flags) {

    auto wic_rect = rect.ToWICRect();
    IWICBitmapLock* handle = nullptr;
    HRESULT com_error = GetHandle()->Lock(&wic_rect, static_cast<DWORD>(flags), &handle);

    ZAF_THROW_IF_COM_ERROR(com_error);
    return Lock{ handle };
}


Size Bitmap::Lock::GetSize() const {

    UINT width = 0;
    UINT height = 0;
    HRESULT com_error = GetHandle()->GetSize(&width, &height);

    ZAF_THROW_IF_COM_ERROR(com_error);
    return Size(static_cast<float>(width), static_cast<float>(height));
}


std::uint32_t Bitmap::Lock::GetStride() const {

    UINT stride = 0;
    HRESULT com_error = GetHandle()->GetStride(&stride);

    ZAF_THROW_IF_COM_ERROR(com_error);
    return stride;
}


void Bitmap::Lock::GetDataPointer(std::uint8_t*& data_pointer, std::size_t& data_size) const {

    UINT buffer_size{};
    HRESULT com_error = GetHandle()->GetDataPointer(&buffer_size, &data_pointer);
    ZAF_THROW_IF_COM_ERROR(com_error);

    data_size = buffer_size;
}

}