#include <zaf/graphic/wic/bitmap.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/wic/palette.h>

namespace zaf::wic {

void Bitmap::SetResolution(double x, double y) {

    HRESULT com_error = Inner()->SetResolution(x, y);
    ZAF_THROW_IF_COM_ERROR(com_error);
}


void Bitmap::SetPalette(const Palette& palette) {

    HRESULT com_error = Inner()->SetPalette(palette.Ptr().Inner());
    ZAF_THROW_IF_COM_ERROR(com_error);
}


Bitmap::Lock Bitmap::GetLock(const Rect& rect, LockFlag flags) {

    auto wic_rect = rect.ToWICRect();
    COMPtr<IWICBitmapLock> inner;
    HRESULT com_error = Inner()->Lock(&wic_rect, static_cast<DWORD>(flags), inner.Reset());

    ZAF_THROW_IF_COM_ERROR(com_error);
    return Lock{ inner };
}


Size Bitmap::Lock::GetSize() const {

    UINT width = 0;
    UINT height = 0;
    HRESULT com_error = Ptr()->GetSize(&width, &height);

    ZAF_THROW_IF_COM_ERROR(com_error);
    return Size(static_cast<float>(width), static_cast<float>(height));
}


std::uint32_t Bitmap::Lock::GetStride() const {

    UINT stride = 0;
    HRESULT com_error = Ptr()->GetStride(&stride);

    ZAF_THROW_IF_COM_ERROR(com_error);
    return stride;
}


void Bitmap::Lock::GetDataPointer(std::uint8_t*& data_pointer, std::size_t& data_size) const {

    UINT buffer_size{};
    HRESULT com_error = Ptr()->GetDataPointer(&buffer_size, &data_pointer);
    ZAF_THROW_IF_COM_ERROR(com_error);

    data_size = buffer_size;
}

}